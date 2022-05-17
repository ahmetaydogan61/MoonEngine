#include "mpch.h"
#include "Application.h"
#include "Window.h"
#include "Time.h"
#include "ImGuiLayer.h"

namespace MoonEngine
{
	Application* Application::m_Instance;

	Application::Application()
	{
		if (!m_Instance)
		{
			m_Instance = this;

			if (Window::Create("MoonEngine", 1280, 720))
				DebugSys("Window Creation Successful");
			else
				DebugSys("Window Creation Failed!");
		}
		else
			DebugSys("Application Already Exists");

		Window::SetEventCallback(BIND_FN(Application::OnEvent));
	}

	void Application::OnEvent(Event& e)
	{
		for (auto l = m_Layers.end(); l != m_Layers.begin();)
			(*--l)->OnEvent(e);
	}

	void Application::Run()
	{
		DebugSys("Application Run Started");
		srand(static_cast <unsigned> (time(0)));

		for (Layer* layer : m_Layers)
		{
			layer->Create();
			DebugSys(layer->LayerName + " Created");
		}
		DebugSys("All Layers Created");

		ImGuiLayer::Init();

		Time::Init();

		while (Window::IsRunning())
		{
			Time::Calculate();

			for (Layer* layer : m_Layers)
				layer->Update();

			ImGuiLayer::BeginDrawUI();
			for (Layer* layer : m_Layers)
				layer->DrawGUI();
			ImGuiLayer::EndDrawUI();

			Window::Update();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		layer->Destroy();
		m_Layers.remove(layer);
	}

	Application::~Application()
	{
		Window::Destroy();
		
		for (Layer* layer : m_Layers)
			layer->Destroy();
		for (Layer* layer : m_Layers)
			delete layer;
		m_Layers.clear();
		
		DebugSys("Application Closed");
	}
}