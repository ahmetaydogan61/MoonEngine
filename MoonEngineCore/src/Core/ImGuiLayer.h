#pragma once
namespace MoonEngine
{
	struct ImGuiLayer
	{
		static glm::vec2 ViewportPosition;
		static glm::vec2 ViewportSize;
		static glm::mat4 CameraProjection;
		static void Init();
		static void OnEvent(Event& event);
		static void BeginDrawGUI();
		static void EndDrawGUI();
		static void BlockEvent(bool state);
	private:
		static bool m_EventsBlocked;
	};
}