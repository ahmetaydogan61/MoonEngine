#include <mpch.h>
#include "Views/Viewport/Viewport.h"
#include "Editor/EditorAssets.h"
#include "Editor/EditorLayer.h"

#include <Core/Time.h>
#include <Engine/Components.h>
#include <Engine/Entity.h>
#include <Utils/Maths.h>
#include <Gui/ImGuiUtils.h>

#include <imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <IconsMaterialDesign.h>


namespace MoonEngine
{
	ViewportView::ViewportView()
	{
		Name = ICON_MD_CAMERA;
		Name += "Viewport";
		Flags = ImGuiWindowFlags_NoScrollbar;

		m_GizmosData.ShowGizmos = true;
		m_GizmosData.GizmosColor = { 0.0f, 0.6f, 1.0f, 1.0f };

		FramebufferProps props = { {FramebufferTextureFormat::RGBA8}, {FramebufferTextureFormat::RED_INTEGER}, {FramebufferTextureFormat::DEPTH} };
		Viewbuffer = MakeShared<Framebuffer>(props);
		m_EditorCamera = MakeShared<EditorCamera>();
		m_EditorCamera->Zoom(2.5f);
	}

	void ViewportView::Update()
	{
		if (!Enabled)
			return;

		auto& registry = GetRegistry();
		auto& selectedEntity = EditorLayer::Get()->GetSelectedEntity();

		const auto& viewportSize = ViewSize;
		if (viewportSize.x != Viewbuffer->GetWidth() || viewportSize.y != Viewbuffer->GetHeight())
		{
			Viewbuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_EditorCamera->Resize(viewportSize.x / viewportSize.y);
		}

		m_EditorCamera->Update(ViewFocused, ViewHovered);

		m_GizmosData.IsSnapping = Input::GetKey(Keycode::LeftControl);

		//+Render Viewport
		Viewbuffer->Bind();

		Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f });
		Renderer::Begin(m_EditorCamera->GetViewProjection());
		Viewbuffer->ClearColorAttachment(1, (void*)-1);

		//SpriteRenderer
		auto view = registry.view<const TransformComponent, const SpriteComponent>();
		for (auto [entity, transform, sprite] : view.each())
			Renderer::DrawEntity(transform, sprite, (int)entity);

		Renderer::SetLineWidth(2.0f);

		//ParticleSystem
		auto particleSystemView = registry.view<const TransformComponent, ParticleComponent>();
		float dt = Time::DeltaTime();
		bool isEditorPlaying = EditorLayer::State() == EditorLayer::EditorState::Play;

		for (auto [entity, transformComponent, particle] : particleSystemView.each())
		{
			Entity e{ entity, Scene };
			if (selectedEntity == e)
			{
				if (!isEditorPlaying)
				{
					if (particle.ParticleSystem.IsPaused())
						particle.ParticleSystem.Play();

					particle.ParticleSystem.UpdateEmitter(dt, particle.Particle, transformComponent.Position);
					particle.ParticleSystem.UpdateParticles(dt);
				}

				switch (particle.ParticleSystem.EmitterType)
				{
					case EmitterType::Box:
					{
						Renderer::DrawRect(transformComponent.Position + particle.Particle.SpawnPosition, particle.Particle.SpawnRadius,
										   m_GizmosData.GizmosColor, (int)entity);
						break;
					}
					case EmitterType::Cone:
					{
						int ent = (int)entity;
						const auto& color = m_GizmosData.GizmosColor;

						const glm::vec3& spawnPos = transformComponent.Position + particle.Particle.SpawnPosition;
						const glm::vec3& tipPos = spawnPos + glm::vec3(0.0f, particle.Particle.SpawnRadius.y, 0.0f);

						Renderer::DrawLine(spawnPos, tipPos, color, ent);

						float spawnRadiusX = particle.Particle.SpawnRadius.x * 0.5f;
						const glm::vec3& spawnRadius = glm::vec3(spawnRadiusX, 0.0f, 0.0f);

						const glm::vec3& p0 = spawnPos - spawnRadius;
						const glm::vec3& p1 = spawnPos + spawnRadius;
						Renderer::DrawLine(p0, p1, color, ent);

						float coneRadiusSize = particle.Particle.DirectionRadiusFactor * 0.5f;
						const glm::vec3& coneRadius = glm::vec3(coneRadiusSize, 0.0f, 0.0f);

						const glm::vec3& p3 = tipPos - spawnRadius - coneRadius;
						const glm::vec3& p4 = tipPos + spawnRadius + coneRadius;
						Renderer::DrawLine(p3, p4, color, ent);

						Renderer::DrawLine(p0, p3, color, ent);
						Renderer::DrawLine(p1, p4, color, ent);
					}
					default:
						break;
				}
			}
			else
			{
				if (particle.ParticleSystem.IsPlaying() && !isEditorPlaying)
					particle.ParticleSystem.Pause();
			}

			particle.ParticleSystem.DrawParticles((int)entity);
		}

		Renderer::End();

		if (m_GizmosData.ShowGizmos)
		{
			Renderer::SetLineWidth(m_GizmosData.LineWidth);

			//GIZMO_CameraComponent
			auto cameraView = registry.view<const TransformComponent, const CameraComponent>();
			for (auto [entity, transformComponent, cameraComponent] : cameraView.each())
			{
				const glm::mat4& transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
					* glm::scale(glm::mat4(1.0f), glm::vec3(m_GizmosData.IconSize, m_GizmosData.IconSize, 0.0f));

				Renderer::DrawEntity(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, EditorAssets::CameraTexture, 0, { 1.0f, 1.0f }, (int)entity);
			}

			//GIZMO_ParticleSystem
			auto particleSystemView = registry.view<const TransformComponent, ParticleComponent>();
			for (auto [entity, transformComponent, particle] : particleSystemView.each())
			{
				if (!particle.ParticleSystem.IsPlaying() && !particle.ParticleSystem.IsPaused())
				{
					const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
						* glm::scale(glm::mat4(1.0f), glm::vec3(m_GizmosData.IconSize, m_GizmosData.IconSize, 0.0f));;
					Renderer::DrawEntity(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, EditorAssets::FlareTexture, 0, { 1.0f, 1.0f }, (int)entity);
				}
			}

			//GIZMO_AllBoxCollider
			if (m_GizmosData.ShowAllColliders)
			{
				auto pbView = registry.view<const TransformComponent, const PhysicsBodyComponent>();
				for (auto [entity, transformComponent, pbComponent] : pbView.each())
				{
					const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
					glm::vec3 scale = glm::vec3(1.0f);
					scale.x = pbComponent.Size.x * 2.0f * transformComponent.Scale.x;
					scale.y = pbComponent.Size.y * 2.0f * transformComponent.Scale.y;
					const glm::mat4& transform =
						glm::translate(glm::mat4(1.0f), transformComponent.Position + glm::vec3(pbComponent.Offset, 0.0f))
						* rotationMat * glm::scale(glm::mat4(1.0f), scale);

					Renderer::DrawRect(transform, { 0.5f, 0.9f, 0.5f, 1.0f });
				}
			}

			Renderer::End();

			//GIZMO_SELECT
			if (selectedEntity)
			{
				TransformComponent& transformComponent = selectedEntity.GetComponent<TransformComponent>();
				const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
				const glm::mat4& transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
					* rotationMat * glm::scale(glm::mat4(1.0f), transformComponent.Scale);

				if (m_GizmosData.HighlightSelected && !selectedEntity.HasComponent<CameraComponent>() && !selectedEntity.HasComponent<ParticleComponent>())
					Renderer::DrawRect(transform, m_GizmosData.GizmosColor);

				if (selectedEntity.HasComponent<PhysicsBodyComponent>())
				{
					const auto& pbComponent = selectedEntity.GetComponent<PhysicsBodyComponent>();

					glm::vec3 position = transformComponent.Position;
					position.x += pbComponent.Offset.x;
					position.y += pbComponent.Offset.y;

					glm::vec3 scale = glm::vec3(1.0f);
					const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));

					scale.x = pbComponent.Size.x * 2.0f * transformComponent.Scale.x;
					scale.y = pbComponent.Size.y * 2.0f * transformComponent.Scale.y;
					const glm::mat4& transform =
						glm::translate(glm::mat4(1.0f), position)
						* rotationMat * glm::scale(glm::mat4(1.0f), scale);

					Renderer::DrawRect(transform, { 0.0f, 1.0f, 0.0f, 1.0f });
				}
			}

			Renderer::End();
		}

		//+MousePicking
		if (ViewHovered)
		{
			const auto& viewportPositon = ViewPosition;
			ImVec2 mousePos = ImGui::GetMousePos();
			mousePos.x -= viewportPositon.x;
			mousePos.y -= viewportPositon.y;
			mousePos.y = viewportSize.y - mousePos.y;

			int mouseX = (int)mousePos.x;
			int mouseY = (int)mousePos.y;

			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && (int)mouseY < viewportSize.y && Input::GetMouseButtonDown(0))
			{
				int pixelData = Viewbuffer->ReadPixel(1, mouseX, mouseY);
				if (!m_GizmosData.IsUsing)
					selectedEntity = pixelData == -1 ? Entity{} : Entity{ (entt::entity)pixelData, Scene };
			}
		}
		//-MousePicking


		Viewbuffer->Unbind();
		//-Render Viewport
	}

	void ViewportView::Render()
	{
		if (!Enabled)
			return;

		auto& selectedEntity = EditorLayer::Get()->GetSelectedEntity();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(Name.c_str(), &Enabled, Flags);

		float fontSize = ImGui::GetFontSize();
		float padY = fontSize * 0.1f;
		float buttonSize = fontSize * 0.8f + 2.0f;

		float x = ImGui::GetContentRegionAvail().x;

		int childFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration;

		ImGui::BeginChild("Chil Window", {0.0f, buttonSize * 2.0f}, false, childFlags);
		{//Gizmo Button Group

			ImGuiUtils::AddPadding(fontSize * 0.3f, buttonSize * 0.35f);

			ImGui::BeginGroup();

			if (ImGuiUtils::ButtonSelectable((ImTextureID)EditorAssets::SelectTexture->GetTextureId(), buttonSize, buttonSize, m_GizmosData.GizmoSelection == GizmoSelection::NONE))
				m_GizmosData.GizmoSelection = GizmoSelection::NONE;

			ImGui::SameLine(0.0f, fontSize * 0.3f);

			if (ImGuiUtils::ButtonSelectable((ImTextureID)EditorAssets::TranslateTexture->GetTextureId(), buttonSize, buttonSize, m_GizmosData.GizmoSelection == GizmoSelection::TRANSLATE))
				m_GizmosData.GizmoSelection = GizmoSelection::TRANSLATE;

			ImGui::SameLine(0.0f, fontSize * 0.3f);

			if (ImGuiUtils::ButtonSelectable((ImTextureID)EditorAssets::ResizeTexture->GetTextureId(), buttonSize, buttonSize, m_GizmosData.GizmoSelection == GizmoSelection::SCALE))
				m_GizmosData.GizmoSelection = GizmoSelection::SCALE;

			ImGui::SameLine(0.0f, fontSize * 0.3f);

			if (ImGuiUtils::ButtonSelectable((ImTextureID)EditorAssets::RotateTexture->GetTextureId(), buttonSize, buttonSize, m_GizmosData.GizmoSelection == GizmoSelection::RORTATE))
				m_GizmosData.GizmoSelection = GizmoSelection::RORTATE;

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - padY - buttonSize * 2.0f);

			if (ImGuiUtils::ButtonSelectable((ImTextureID)EditorAssets::SettingsTexture->GetTextureId(), buttonSize, buttonSize, false))
				ImGui::OpenPopup("GizmoSettingsPopup");

			if (ImGui::BeginPopup("GizmoSettingsPopup", ImGuiWindowFlags_NoMove))
			{
				ImGuiUtils::AddPadding(fontSize * 0.25f, fontSize * 0.25f);

				ImGui::BeginGroup();

				ImGuiUtils::Label("Icon Size: ");
				ImGui::SliderFloat("##is", &m_GizmosData.IconSize, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

				ImGuiUtils::Label("Outline Width: ");
				ImGui::SliderFloat("##ow", &m_GizmosData.LineWidth, 1.0f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

				ImGuiUtils::Label("Color: ");
				ImGui::ColorEdit4("##t", &m_GizmosData.GizmosColor[0]);

				ImGuiUtils::Label("Show All Colliders: ");
				ImGui::Checkbox("##sac", &m_GizmosData.ShowAllColliders);

				ImGuiUtils::Label("Highlight Selected: ");
				ImGui::Checkbox("##hs", &m_GizmosData.HighlightSelected);

				ImGui::EndGroup();

				ImGuiUtils::AddPadding(fontSize * 0.25f, fontSize * 0.25f);

				ImGui::EndPopup();
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - padY - buttonSize * 4.0f);

			if (ImGuiUtils::ButtonSelectable((ImTextureID)EditorAssets::TransformationTexture->GetTextureId(), buttonSize, buttonSize, m_GizmosData.ShowGizmos))
				m_GizmosData.ShowGizmos = !m_GizmosData.ShowGizmos;

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				ImGui::SetTooltip(" Show Gizmos ");

			ImGui::EndGroup();
			ImGuiUtils::AddPadding(0.0f, padY);
		}
		ImGui::EndChild();

		OnWindowBegin();

		ImGui::Image((void*)Viewbuffer->GetTexID(), { ViewSize.x, ViewSize.y }, { 0, 1 }, { 1, 0 });

		auto& GizmoSelection = m_GizmosData.GizmoSelection;
		if (selectedEntity && GizmoSelection != GizmoSelection::NONE)
		{
			const glm::mat4& view = m_EditorCamera->GetView();
			const glm::mat4& projection = m_EditorCamera->GetProjection();

			TransformComponent& component = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 rotation = glm::toMat4(glm::quat(component.Rotation));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), component.Position) * rotation * glm::scale(glm::mat4(1.0f), component.Scale);

			ImGuizmo::SetRect(ViewPosition.x, ViewPosition.y, ViewSize.x, ViewSize.y);
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetDrawlist();

			float snapAmount = 0;

			if (GizmoSelection != GizmoSelection::RORTATE)
				snapAmount = m_GizmosData.SnapAmount;
			else
				snapAmount = 45.0f;

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)GizmoSelection, ImGuizmo::LOCAL, glm::value_ptr(transform), NULL, m_GizmosData.IsSnapping ? &snapAmount : NULL);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 finalPos, finalRot, finalSiz;
				Maths::DecomposeTransform(transform, finalPos, finalRot, finalSiz);
				glm::vec3 deltaRotation = finalRot - component.Rotation;
				finalPos.z = 0.0f;
				component.Position = finalPos;
				component.Rotation += deltaRotation;
				component.Scale = finalSiz;
				m_GizmosData.IsUsing = true;
			}
			else
				m_GizmosData.IsUsing = false;
		}
		else
			m_GizmosData.IsUsing = false;
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void ViewportView::OnKeyEvent(Keycode key)
	{
		if (!ViewHovered)
			return;

		switch (key)
		{
			case Keycode::Q:
			{
				m_GizmosData.GizmoSelection = GizmoSelection::NONE;
				break;
			}
			case Keycode::W:
			{
				m_GizmosData.GizmoSelection = GizmoSelection::TRANSLATE;
				break;
			}
			case Keycode::E:
			{
				m_GizmosData.GizmoSelection = GizmoSelection::SCALE;
				break;
			}
			case Keycode::R:
			{
				m_GizmosData.GizmoSelection = GizmoSelection::RORTATE;
				break;
			}
		}
	}
}
