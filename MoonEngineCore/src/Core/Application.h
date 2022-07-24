#pragma once
#include "Layer.h"

namespace MoonEngine
{
	struct ApplicationDesc
	{
		std::string AppName = "Application";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool Fullscreen = false;
		bool Maximized = false;
	};

	class Application
	{
	private:
		static Application* m_Instance;
		std::vector<Layer*> m_Layers;
		void OnEvent(Event& e);
	public:
		Application();
		Application(ApplicationDesc desc);
		~Application();
		void Run();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	};
}