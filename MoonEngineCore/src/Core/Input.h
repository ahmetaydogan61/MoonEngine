#pragma once
#include "Core/Keycodes.h"

namespace MoonEngine
{
	class Input
	{
	private:
		Input() = delete;
		~Input() = delete;
		static glm::vec4 m_MouseOrthoPos;
		static bool m_MouseButtons[3];
		static bool m_LastMouseButtons[3];

		static void UpdateMouseKeys();
		static void Update(const glm::vec2& viewPortPos, const glm::vec2& viewPortSize, const glm::mat4& viewProjection);

		friend class Application;
	public:
		static bool GetKey(int key);

		static float GetX();
		static float GetY();
		static float OrthoX();
		static float OrthoY();
		
		static bool MouseDown(int button);
		static bool MouseUp(int button);
		static bool MousePressed(int button);
		static bool MouseReleased(int button);
		static bool Dragging(int button);
	};
}