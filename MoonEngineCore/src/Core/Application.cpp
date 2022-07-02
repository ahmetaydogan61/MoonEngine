#include "mpch.h"

#include "Application.h"
#include "ResourceManager.h"
#include "Window.h"
#include "Time.h"
#include "Input.h"
#include "ImGuiLayer.h"

#include "Renderer/Renderer.h"

namespace MoonEngine
{
	Application* Application::m_Instance;

	Application::Application()
	{
		if (!m_Instance)
		{
			m_Instance = this;
			if (Window::Create("MoonEngine", 1600, 900, false))
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
		ImGuiLayer::OnEvent(e);
		for (auto layer = m_Layers.end(); layer != m_Layers.begin();)
			(*--layer)->OnEvent(e);
	}

	void Application::Run()
	{
		DebugSys("Application Run Started");
		srand(static_cast <unsigned> (time(0)));

		ImGuiLayer::Init();
		Renderer::Init();

		DebugSys("---Layer Creation Stared---");
		for (Layer* layer : m_Layers)
		{
			layer->Create();
			DebugSys(layer->LayerName + " Created");
		}
		DebugSys("---Layer Creation Ended---");

		Time::Init();

		while (Window::IsRunning())
		{
			Time::Calculate();

			for (Layer* layer : m_Layers)
				layer->Update();
			
			ImGuiLayer::BeginDrawGUI();
			for (Layer* layer : m_Layers)
				layer->DrawGUI();
			ImGuiLayer::EndDrawGUI();

			Input::Update(ImGuiLayer::ViewportPosition, ImGuiLayer::ViewportSize, Renderer::GetRenderData().ViewProjection);
			Window::Update();

			ResourceManager::CollectGarbage();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		layer->Destroy();
		m_Layers.erase(std::find(m_Layers.begin(), m_Layers.end(), layer));
	}

	Application::~Application()
	{
		Window::Destroy();
		
		for (Layer* layer : m_Layers)
		{
			DebugSys(layer->LayerName + " Destroyed");
			layer->Destroy();
		}
		for (Layer* layer : m_Layers)
			delete layer;
		m_Layers.clear();
		
		Renderer::Destroy();

		DebugSys("Application Closed");
	}
}