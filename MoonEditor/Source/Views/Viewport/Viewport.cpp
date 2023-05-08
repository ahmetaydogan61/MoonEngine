#include <mpch.h>
#include "Views/Viewport/Viewport.h"
#include "Editor/EditorLayer.h"

#include <Core/Time.h>
#include <Engine/Components.h>
#include <Engine/Entity.h>
#include <Utils/Maths.h>

#include <imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <IconsMaterialDesign.h>

namespace MoonEngine
{
	Shared<Texture> cameraTexture;
	Shared<Texture> flareTexture;

	ViewportView::ViewportView()
	{
		Name = ICON_MD_CAMERA;
		Name += "Viewport";
		Flags = ImGuiWindowFlags_NoScrollbar;

		FramebufferProps props = { {FramebufferTextureFormat::RGBA8}, {FramebufferTextureFormat::RED_INTEGER}, {FramebufferTextureFormat::DEPTH} };
		Viewbuffer = MakeShared<Framebuffer>(props);
		m_EditorCamera = MakeShared<EditorCamera>();

		cameraTexture = MakeShared<Texture>("Resource/EditorIcons/Camera.png");
		flareTexture = MakeShared<Texture>("Resource/EditorIcons/Flare.png");
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

		Renderer::SetRenderData(m_EditorCamera->GetViewProjection());
		Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f });
		Renderer::Begin();
		Viewbuffer->ClearColorAttachment(1, (void*)-1);

		//SpriteRenderer
		auto view = registry.view<const TransformComponent, const SpriteComponent>();
		for (auto [entity, transform, sprite] : view.each())
			Renderer::DrawEntity(transform, sprite, (int)entity);

		Renderer::SetLineWidth(2.0f);

		//ParticleSystem
		auto particleSystemView = registry.view<const TransformComponent, ParticleComponent>();
		float dt = Time::DeltaTime();
		for (auto [entity, transformComponent, particle] : particleSystemView.each())
		{
			particle.ParticleSystem.Update(dt, particle.Particle, transformComponent.Position);
			particle.ParticleSystem.UpdateParticles(dt, (int)entity);

			if (!particle.ParticleSystem.IsPlaying() && !particle.ParticleSystem.IsPaused())
			{
				const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Position);
				Renderer::DrawEntity(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, flareTexture, { 1.0f, 1.0f }, (int)entity);
			}

			Entity e{ entity, Scene };
			if (selectedEntity == e)
			{
				switch (particle.ParticleSystem.EmitterType)
				{
					case EmitterType::Box:
					{
						Renderer::DrawRect(transformComponent.Position + particle.Particle.SpawnPosition, particle.Particle.SpawnRadius,
										   { 0.0f, 150.0f / 255.0f, 1.0f, 1.0f }, (int)entity);
						break;
					}
					case EmitterType::Cone:
					{
						int ent = (int)entity;
						const auto& color = glm::vec4{ 0.0f, 150.0f / 255.0f, 1.0f, 1.0f };

						const glm::vec3& spawnPos = transformComponent.Position + particle.Particle.SpawnPosition;
						const glm::vec3& tipPos = spawnPos + glm::vec3(0.0f, particle.Particle.SpawnRadius.y, 0.0f);

						Renderer::DrawLine(spawnPos, tipPos, color, ent);

						float spawnRadiusX = particle.Particle.SpawnRadius.x * 0.5f;
						const glm::vec3& spawnRadius = glm::vec3(spawnRadiusX, 0.0f, 0.0f);

						const glm::vec3& p0 = spawnPos - spawnRadius;
						const glm::vec3& p1 = spawnPos + spawnRadius;
						Renderer::DrawLine(p0, p1, color, ent);

						float coneRadiusSize = particle.Particle.ConeRadius * 0.5f;
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
		}

		Renderer::End();

		//CameraComponent
		auto cameraView = registry.view<const TransformComponent, const CameraComponent>();
		for (auto [entity, transformComponent, cameraComponent] : cameraView.each())
		{
			const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
				* rotationMat * glm::scale(glm::mat4(1.0f), glm::vec3(cameraComponent.Size * 2.0f, cameraComponent.Size * 2.0f, 0.0f));

			transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));

			Renderer::DrawEntity(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, cameraTexture, { 1.0f, 1.0f }, (int)entity);
		}

		Renderer::End();

		Renderer::SetLineWidth(4.0f);

		if (selectedEntity)
		{
			TransformComponent& transformComponent = selectedEntity.GetComponent<TransformComponent>();
			const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
			const glm::mat4& transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
				* rotationMat * glm::scale(glm::mat4(1.0f), transformComponent.Scale);

			if (!selectedEntity.HasComponent<CameraComponent>() && !selectedEntity.HasComponent<ParticleComponent>())
				Renderer::DrawRect(transform, { 0.0f, 150.0f / 255.0f, 1.0f, 1.0f });
		}

		Renderer::End();

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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(Name.c_str(), &Enabled, Flags);
		
		OnWindowBegin();

		ImGui::Image((void*)Viewbuffer->GetTexID(), { ViewSize.x, ViewSize.y }, { 0, 1 }, { 1, 0 });

		auto& gizmoSelection = m_GizmosData.GizmoSelection;
		if (selectedEntity && gizmoSelection != GIZMOSELECTION::NONE)
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

			if (gizmoSelection != GIZMOSELECTION::RORTATE)
				snapAmount = m_GizmosData.SnapAmount;
			else
				snapAmount = 45.0f;

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)gizmoSelection, ImGuizmo::LOCAL, glm::value_ptr(transform), NULL, m_GizmosData.IsSnapping ? &snapAmount : NULL);

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
		switch (key)
		{
			case Keycode::Q:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::NONE;
				break;
			}
			case Keycode::W:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::TRANSLATE;
				break;
			}
			case Keycode::E:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::SCALE;
				break;
			}
			case Keycode::R:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::RORTATE;
				break;
			}
		}
	}
}
