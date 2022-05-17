#include "mpch.h"
#include "Input.h"
#include "Core/Window.h"
#include "Utils/OrthographicCamera.h"
#include "GLFW/glfw3.h"

namespace MoonEngine
{
	glm::vec4 Input::orthoPos;
	bool Input::m_MouseButtons[3];
	bool Input::m_LastMouseButtons[3];

	bool Input::GetKey(int key)
	{
		if (glfwGetKey(Window::GetWindow(), key) == GLFW_PRESS)
			return true;
		return false;
	}

	float Input::GetX()
	{
		double x, y;
		glfwGetCursorPos(Window::GetWindow(), &x, &y);
		return (float)x;
	}

	float Input::GetY()
	{
		double x, y;
		glfwGetCursorPos(Window::GetWindow(), &x, &y);
		return (float)y;
	}

	bool Input::MouseDown(int button)
	{
		return m_MouseButtons[button];
	}

	bool Input::MousePressed(int button)
	{
		return m_MouseButtons[button] && !m_LastMouseButtons[button];
	}

	bool Input::MouseUp(int button)
	{
		return !m_MouseButtons[button];
	}

	bool Input::MouseReleased(int button)
	{
		return !m_MouseButtons[button] && m_LastMouseButtons[button];
	}

	bool Input::Dragging(int button)
	{
		return MouseDown(button);
	}

	float Input::OrthoX()
	{
		return orthoPos.x;
	}

	float Input::OrthoY()
	{
		return orthoPos.y;
	}

	void Input::UpdateMouseKeys()
	{
		for (int i = 0; i < 3; i++)
			m_LastMouseButtons[i] = m_MouseButtons[i];
		int state = glfwGetMouseButton(Window::GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS)
			m_MouseButtons[0] = true;
		else
			m_MouseButtons[0] = false;
		state = glfwGetMouseButton(Window::GetWindow(), GLFW_MOUSE_BUTTON_RIGHT);
		if (state == GLFW_PRESS)
			m_MouseButtons[1] = true;
		else
			m_MouseButtons[1] = false;
		state = glfwGetMouseButton(Window::GetWindow(), GLFW_MOUSE_BUTTON_MIDDLE);
		if (state == GLFW_PRESS)
			m_MouseButtons[2] = true;
		else
			m_MouseButtons[2] = false;
	}

	void Input::Update(glm::vec2 viewPortSize, glm::vec2 viewPortPos)
	{
		UpdateMouseKeys();
		glm::vec2 m_ViewportSize = glm::vec2(viewPortSize.x, viewPortSize.y);
		glm::vec2 m_ViewportPos = glm::vec2(viewPortPos.x, viewPortPos.y);
		glm::mat4 inverseMat = glm::inverse(OrthographicCamera::MAIN->GetViewProjection());
		float x = (2.0f * ((float)(GetX() - m_ViewportPos.x) / (m_ViewportSize.x - 0))) - 1.0f;
		float y = (2.0f * (1.0f - ((float)(GetY() - m_ViewportPos.y) / (m_ViewportSize.y - 0)))) - 1.0f;
		orthoPos = glm::vec4(x, y, 0.0f, 1.0f);
		orthoPos = inverseMat * orthoPos;
	}
}