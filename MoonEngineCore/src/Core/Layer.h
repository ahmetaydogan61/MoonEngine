#pragma once
#include "Event/Events.h"

namespace MoonEngine
{
	class Layer
	{
	public:
		std::string LayerName = "DefaultLayer";
		virtual void Create() {}
		virtual void Update() {}
		virtual void Destroy() {}
		virtual void OnEvent(Event& event) {}
		virtual void DrawGUI() {}
	};
}