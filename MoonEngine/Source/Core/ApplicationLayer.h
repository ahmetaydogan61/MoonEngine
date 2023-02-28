#pragma once

namespace MoonEngine
{
	class ApplicationLayer
	{
	public:
		virtual void Init() {}
		virtual void Update() {}
		virtual void DrawGui() {}
		virtual void Terminate() {}
	};
}