#pragma once
#include "Core/Keycode.h"

#include "Event/Action.h"

struct GLFWwindow;

namespace MoonEngine
{
	class Camera;

	class Input
	{
	public:
		static bool GetKey(Keycode key);

		static bool GetMouseButton(int button);
		static bool GetMouseButtonDown(int button);

		static const glm::vec2 GetMouseScreenPos();

		inline static Action<float> OnMouseScroll;
		inline static Action<KeyPressEvent&> OnKeyPress;
	private:
		static bool s_MouseButtons[3];
		static bool s_LastMouseButtons[3];

		//Needs to be called from application update
		static void Update();

		friend class Application;
	};
}