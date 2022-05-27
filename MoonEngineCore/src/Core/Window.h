#pragma once
struct GLFWwindow;

namespace MoonEngine
{
	class Event;
	
	class Window
	{
	private:
		Window() = delete;
		static GLFWwindow* m_GLWindow;
		static std::string m_Name;
		static uint32_t m_Width;
		static uint32_t m_Height;
		static bool m_Vsync;
		static std::function<void(Event&)> m_EventCallback;
	public:
		static int Create(std::string name, uint32_t width, uint32_t height, bool fullscreen);
		static void Update();
		static void Destroy();
		
		static void SetEventCallback(const std::function<void(Event&)>& e);
		static bool IsRunning();
		static void SetVsync(bool state);
		static GLFWwindow* GetWindow() { return m_GLWindow; }

		static uint32_t GetWidth() { return m_Width; }
		static uint32_t GetHeight() { return m_Height; }
		static float GetAspectRatio() { return (float) m_Width / (float) m_Height; }
	};
}