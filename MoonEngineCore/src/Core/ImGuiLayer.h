#pragma once
namespace MoonEngine
{
	struct ImGuiLayer
	{
		static glm::vec2 ViewportSize;
		static glm::vec2 ViewportPosition;
		static void Init();
		static void OnEvent(Event& event);
		static void BeginDrawUI();
		static void EndDrawUI();
		static void BlockEvent(bool state);
	private:
		static bool m_EventsAllowed;
		static bool OnMouseScroll(Event& event);
		static void StyleCustomDark(int is3D = 0);
	};
}