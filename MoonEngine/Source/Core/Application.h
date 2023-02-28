#pragma once
#include "Core/ApplicationLayer.h"
#include "Core/Debug.h"
#include "Core/Window.h"

#include "Gui/ImGuiLayer.h"

namespace MoonEngine
{
	struct Resolution
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		float Dpi = 1.0f;
	};

	struct ApplicationPrefs
	{
		const char* AppName = "DemoApp";

		Resolution Resolution{ 1280, 720 };

		bool VsyncOn = true;
		bool MaximizeOnStart = false;
	};

	class Application
	{
	private:
		ApplicationPrefs m_Prefs;
		Debug* m_Debug;
		ImGuiLayer* m_ImGuiLayer;
		Window* m_Window;
		std::vector<Shared<ApplicationLayer>> m_ApplicationLayers;
		
		bool m_IsRunning = false;

		static Application* s_Instance;
		
		void Init();
		void LoadPrefs();
	public:
		Application(ApplicationPrefs prefs);
		Application(const Application&) = delete;
		Application& operator= (const Application&) = delete;
		~Application();

		void Run();
		void Terminate();
		void PushLayer(const Shared<ApplicationLayer>& layer) { m_ApplicationLayers.push_back(layer); }
		void PopLayer(const Shared<ApplicationLayer>& layer) { m_ApplicationLayers.erase(std::find(m_ApplicationLayers.begin(), m_ApplicationLayers.end(), layer)); }

		static bool IsRunning() { return s_Instance && s_Instance->m_IsRunning; }
		static Application* GetApp() { return s_Instance; }
		static ApplicationPrefs& GetPrefs() { return s_Instance->m_Prefs; }
		static void SavePrefs();

		static const Resolution& GetResoultion() { return GetPrefs().Resolution; }
		static GLFWwindow* GetWindow() { return s_Instance->m_Window->GetNative(); }
		
		static void SetVsync(bool state) { GetPrefs().VsyncOn = state; s_Instance->m_Window->SetVsync(state); }
	};
}