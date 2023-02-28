#pragma once
#include "Core/Keycode.h"

#include "Event/Action.h"

struct GLFWwindow;

namespace MoonEngine
{
	class Input
	{
	private:
		static bool s_MouseButtons[3];
		static bool s_LastMouseButtons[3];
		static glm::vec2 s_OrthoMousePos;

		//Needs to be called from application update
		static void Update();

		friend class Application;
	public:
		static bool GetKey(Keycode key);

		static bool GetMouseButton(int button);
		static bool GetMouseButtonDown(int button);

		static const glm::vec2 GetMouseScreenPos();
		static const glm::vec2 GetMouseWorldPos();

		inline static Action<float> OnMouseScroll;
		inline static Action<KeyPressEvent&> OnKeyPress;
	};
}