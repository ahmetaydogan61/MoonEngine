#include <mpch.h>
#include "GameView.h"
#include "Editor/EditorLayer.h"

#include <Engine/Components.h>

#include <IconsMaterialDesign.h>

namespace MoonEngine
{
	GameView::GameView()
	{
		Name = ICON_MD_LANDSCAPE;
		Name += "GameView";
		Flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
		Enabled = false;

		FramebufferProps props = { {FramebufferTextureFormat::RGBA8}, {FramebufferTextureFormat::DEPTH} };
		m_Gamebuffer = MakeShared<Framebuffer>(props);
	}

	void GameView::Update()
	{
		if (!Enabled)
			return;

		auto& registry = GetRegistry();

		const auto& viewportSize = ViewSize;
		if (viewportSize.x != m_Gamebuffer->GetWidth() || viewportSize.y != m_Gamebuffer->GetHeight())
		{
			m_Gamebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		}

		CameraComponent* sceneCamera = nullptr;
		glm::vec3 cameraPosition = glm::vec3(0.0f);

		auto cameraView = registry.view<CameraComponent, const TransformComponent>();
		cameraView.each([&](auto& camera, const auto& transform)
		{
			{
				if (camera.IsMain)
				{
					sceneCamera = &camera;
					cameraPosition = transform.Position;
					return;
				}
			}
		});

		glm::mat4 viewProjection;
		if (sceneCamera)
		{
			sceneCamera->Resize(viewportSize.x / viewportSize.y);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 0.0f));
			glm::mat4 m_View = glm::inverse(transform);
			viewProjection = sceneCamera->GetProjection() * m_View;
		}

		//+Render Game
		m_Gamebuffer->Bind();

		Renderer::SetRenderData(viewProjection);
		Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f });
		Renderer::Begin();

		//SpriteRenderer
		{
			auto view = registry.view<const TransformComponent, const SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each())
				Renderer::DrawEntity(transform, sprite, (int)entity);
		}

		//ParticleSystem
		{
			auto view = registry.view<ParticleComponent>();
			for (auto [entity, particle] : view.each())
				particle.ParticleSystem.DrawParticles((int)entity);
		}

		Renderer::End();
		m_Gamebuffer->Unbind();
	}

	void GameView::Render()
	{
		if (!Enabled)
			return;

		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(Name.c_str(), &Enabled, Flags);

		OnWindowBegin();

		ImGui::Image((void*)m_Gamebuffer->GetTexID(), { ViewSize.x, ViewSize.y }, { 0, 1 }, { 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}
}
