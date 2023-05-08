#pragma once
#include <Renderer/Framebuffer.h>
#include <Engine/Scene.h>
#include <imgui.h>

namespace MoonEngine
{
	struct BasicView
	{
		Scene* Scene;
		std::string Name = "Basic View";
		int Flags = 0;
		bool Enabled = true;
		virtual void Render() {}

		void GetMenuItem()
		{
			if (ImGui::MenuItem(Name.c_str(), " ", Enabled, true))
				Enabled = !Enabled;
		}

	protected:
		entt::registry& GetRegistry() { return Scene->m_Registry; }
	};

	struct AdvancedView : public BasicView
	{
		bool ViewFocused = false;
		bool ViewHovered = false;

		glm::vec2 ViewSize = glm::vec2(0.0f);
		glm::vec2 ViewPosition = glm::vec2(0.0f);

		void OnWindowBegin()
		{
			ViewFocused = ImGui::IsWindowFocused();
			ViewHovered = ImGui::IsWindowHovered();

			const auto& viewportSize = ImGui::GetContentRegionAvail();
			ViewSize.x = viewportSize.x;
			ViewSize.y = viewportSize.y;

			const auto& viewportPos = ImGui::GetCursorScreenPos();
			ViewPosition.x = viewportPos.x;
			ViewPosition.y = viewportPos.y;
		}
	};
}
