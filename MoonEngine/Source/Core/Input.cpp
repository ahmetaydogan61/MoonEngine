#include "mpch.h"
#include "Core/Input.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace MoonEngine
{
	bool Input::s_MouseButtons[3];
	bool Input::s_LastMouseButtons[3];
	glm::vec2 Input::s_OrthoMousePos = glm::vec2(0.0f);

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

	const glm::vec2 Input::GetMouseWorldPos()
	{
		return s_OrthoMousePos;
	}

	void Input::Update()
	{
		for (int i = 0; i < 3; i++)
			s_LastMouseButtons[i] = s_MouseButtons[i];

		for (int i = 0; i < 3; i++)
			s_MouseButtons[i] = glfwGetMouseButton(Application::GetWindow(), (int)i);

		double x, y;
		glfwGetCursorPos(Application::GetWindow(), &x, &y);

		Resolution res = Application::GetResoultion();
		double newX = x / (res.Width / 2.0f) - 1.0f;
		double newY = -1 * (y / res.Height * 2.0f - 1.0f);

		glm::mat4 inverseProjection = glm::inverse(glm::mat4(1.0f));
		glm::vec4 orthoPos = { newX, newY, 0.0f, 1.0f };
		s_OrthoMousePos = inverseProjection * orthoPos;
	}
}