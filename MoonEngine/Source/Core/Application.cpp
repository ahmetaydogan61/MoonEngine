#include "mpch.h"
#include "Core/Application.h"

#include "Core/Debug.h"
#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Renderer.h"

#include <GLFW/glfw3.h>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/dll.h>

namespace MoonEngine
{
	Application* Application::s_Instance;

	Application::Application(ApplicationPrefs prefs)
		:m_Prefs(prefs), m_ImGuiLayer(nullptr), m_Window(nullptr)
	{
		Init();
	}

	void Application::Init()
	{
		Debug::Init();

		if (s_Instance)
		{
			ME_SYS_WAR("Application already exists!");
			return;
		}
		s_Instance = this;

		LoadPrefs();
		m_Window = new Window();
		bool windowCreated = m_Window->Init();
		if (!windowCreated)
		{
			ME_SYS_ERR("Window creation failed!");
			return;
		}
		ME_SYS_SUC("Window Created...");

		Renderer::Init();
		ME_SYS_SUC("Renderer Initialized...");

		m_ImGuiLayer = new ImGuiLayer();
		m_ImGuiLayer->Init();
		ME_SYS_SUC("ImGui Initialized...");
	}

	void Application::Run()
	{
		for (auto& layer : m_ApplicationLayers)
			layer->Init();
		ME_SYS_SUC("Application Layers Initialized...");

		m_IsRunning = true;
		ME_SYS_SUC("Application Run Started...");

		Time time{};

		while (m_IsRunning)
		{
			time.Calculate((float)glfwGetTime());

			for (auto& layer : m_ApplicationLayers)
				layer->Update();

			m_ImGuiLayer->BeginDrawGUI();
			for (auto& layer : m_ApplicationLayers)
				layer->DrawGui();
			m_ImGuiLayer->EndDrawGUI();

			Input::Update();
			m_Window->Update();
		}
	}

	void Application::SavePrefs()
	{
		const auto& prefs = Application::GetPrefs();

		const char* filepath = "Resource/Settings/ApplicationPrefs.txt";

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Application Prefs";

		out << YAML::BeginMap;
		out << YAML::Key << "ResolutionWidth" << YAML::Value << prefs.Window.Resolution.Width;
		out << YAML::Key << "ResolutionHeight" << YAML::Value << prefs.Window.Resolution.Height;
		out << YAML::Key << "VsyncOn" << YAML::Value << prefs.Window.VsyncOn;
		out << YAML::Key << "Fullscreen" << YAML::Value << prefs.Window.Fullscreen;
		out << YAML::Key << "MaximizeOnStart" << YAML::Value << prefs.Window.MaximizeOnStart;
		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void Application::LoadPrefs()
	{
		std::string filepath = "Resource/Settings/ApplicationPrefs.txt";

		if (!std::filesystem::exists(filepath))
			return;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return;
		}

		ApplicationPrefs prefs;
		auto yamlPrefs = data["Application Prefs"];
		prefs.AppName = m_Prefs.AppName;
		prefs.Window.Resolution.Width = yamlPrefs["ResolutionWidth"].as<uint32_t>();
		prefs.Window.Resolution.Height = yamlPrefs["ResolutionHeight"].as<uint32_t>();
		prefs.Window.VsyncOn = yamlPrefs["VsyncOn"].as<bool>();
		prefs.Window.Fullscreen = yamlPrefs["Fullscreen"].as<bool>();
		prefs.Window.MaximizeOnStart = yamlPrefs["MaximizeOnStart"].as<bool>();
		m_Prefs = prefs;
	}

	void Application::Terminate()
	{
		SavePrefs();

		m_IsRunning = false;

		m_Window->Terminate();
		ME_SYS_LOG("Window Terminated...");

		for (const auto& layer : m_ApplicationLayers)
			layer->Terminate();
		ME_SYS_LOG("Application Layers Terminated...");

		Renderer::Terminate();

		ME_SYS_LOG("Application Termination Completed..");
		Debug::Terminate();
	}

	Application::~Application()
	{
		delete m_ImGuiLayer;
		delete m_Window;

		s_Instance = nullptr;
	}
}