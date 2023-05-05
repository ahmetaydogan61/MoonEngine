#include "mpch.h"
#include "Core/Input.h"

#include "Core/Application.h"

#include "Renderer/Camera.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace MoonEngine
{
	bool Input::s_MouseButtons[3];
	bool Input::s_LastMouseButtons[3];

	bool Input::GetKey(Keycode key)
	{
		return glfwGetKey(Application::GetWindow(), (int)key);
	}

	bool Input::GetMouseButton(int button)
	{
		return s_MouseButtons[button];
	}

	bool Input::GetMouseButtonDown(int button)
	{
		return s_MouseButtons[button] && !s_LastMouseButtons[button];
	}

	const glm::vec2 Input::GetMouseScreenPos()
	{
		double x, y;
		glfwGetCursorPos(Application::GetWindow(), &x, &y);
		return glm::vec2(x, y);
	}

	void Input::Update()
	{
		for (int i = 0; i < 3; i++)
			s_LastMouseButtons[i] = s_MouseButtons[i];

		for (int i = 0; i < 3; i++)
			s_MouseButtons[i] = glfwGetMouseButton(Application::GetWindow(), (int)i);
	}
}