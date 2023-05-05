#pragma once
#include "Core/ApplicationLayer.h"
#include "Core/Debug.h"
#include "Core/Window.h"

#include "Gui/ImGuiLayer.h"

namespace MoonEngine
{
	struct Resolution
	{
		uint32_t Width = 1280;
		uint32_t Height = 720;
	};

	struct WindowPrefs
	{
		Resolution Resolution;
		float Dpi = 1.0f;
		bool VsyncOn = true;
		bool Fullscreen = false;
		bool MaximizeOnStart = false;
	};

	struct ApplicationPrefs
	{
		const char* AppName = "DemoApp";
		WindowPrefs Window;
	};

	class Application
	{
	public:
		Application(ApplicationPrefs prefs);
		Application(const Application&) = delete;
		Application& operator= (const Application&) = delete;
		~Application();

		static void Quit() { s_Instance->m_IsRunning = false; }
		void Run();
		void Terminate();

		void PushLayer(const Shared<ApplicationLayer>& layer) { m_ApplicationLayers.push_back(layer); }
		void PopLayer(const Shared<ApplicationLayer>& layer) { m_ApplicationLayers.erase(std::find(m_ApplicationLayers.begin(), m_ApplicationLayers.end(), layer)); }

		static bool IsRunning() { return s_Instance && s_Instance->m_IsRunning; }
		static Application* GetApp() { return s_Instance; }
		static ApplicationPrefs& GetPrefs() { return s_Instance->m_Prefs; }
		static WindowPrefs& GetWindowPrefs() { return s_Instance->m_Prefs.Window; }
		static void SavePrefs();

		static const Resolution& GetResoultion() { return GetPrefs().Window.Resolution; }
		static GLFWwindow* GetWindow() { return s_Instance->m_Window->GetNative(); }

		static void SetVsync(bool state) { GetPrefs().Window.VsyncOn = state; s_Instance->m_Window->SetVsync(state); }

	private:
		static Application* s_Instance;

		ApplicationPrefs m_Prefs;
		ImGuiLayer* m_ImGuiLayer;
		Window* m_Window;
		std::vector<Shared<ApplicationLayer>> m_ApplicationLayers;

		bool m_IsRunning = false;

		void Init();
		void LoadPrefs();
	};
}