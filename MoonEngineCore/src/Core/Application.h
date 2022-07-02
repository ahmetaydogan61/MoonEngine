#pragma once
#include "Layer.h"

namespace MoonEngine
{
	class Application
	{
	private:
		static Application* m_Instance;
		std::vector<Layer*> m_Layers;
		void OnEvent(Event& e);
	public:
		Application();
		~Application();
		void Run();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	};
}