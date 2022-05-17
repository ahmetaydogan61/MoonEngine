#pragma once
#include "Keycodes.h"
namespace MoonEngine
{
	class Input
	{
	private:
		static glm::vec4 orthoPos;
		static bool m_MouseButtons[3];
		static bool m_LastMouseButtons[3];
		Input();
		static void Update(glm::vec2 viewPortSize, glm::vec2 viewPortPos);
		friend class Application;
	public:
		static float GetX();
		static float GetY();
		static float OrthoX();
		static float OrthoY();
		static void UpdateMouseKeys();
		static bool GetKey(int key);
		static bool MouseDown(int button);
		static bool MouseUp(int button);
		static bool MousePressed(int button);
		static bool MouseReleased(int button);
		static bool Dragging(int button);
	};
}