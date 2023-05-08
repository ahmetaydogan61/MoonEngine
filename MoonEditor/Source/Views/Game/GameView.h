#pragma once
#include "Views/Views.h"

namespace MoonEngine
{
	struct GameView : public AdvancedView
	{
		GameView();
		~GameView() = default;

		void Update();
		void Render();

		Shared<Framebuffer> m_Gamebuffer;
	};
}