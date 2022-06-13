#include "mpch.h"
#include "Input.h"
#include "Core/Window.h"
#include "GLFW/glfw3.h"

namespace MoonEngine
{
	glm::vec4 Input::m_MouseOrthoPos;
	bool Input::m_MouseButtons[3];
	bool Input::m_LastMouseButtons[3];

	bool Input::GetKey(int key)
	{
		return glfwGetKey(Window::GetWindow(), key);
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
		return m_MouseOrthoPos.x;
	}

	float Input::OrthoY()
	{
		return m_MouseOrthoPos.y;
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

	void Input::Update(const glm::vec2& viewPortPos, const  glm::vec2& viewPortSize, const  glm::mat4& viewProjection)
	{
		UpdateMouseKeys();
		glm::mat4 inverseMat = glm::inverse(viewProjection);
		float x = (2.0f * ((float)(GetX() - viewPortPos.x) / (viewPortSize.x - 0))) - 1.0f;
		float y = (2.0f * (1.0f - ((float)(GetY() - viewPortPos.y) / (viewPortSize.y - 0)))) - 1.0f;
		m_MouseOrthoPos = glm::vec4(x, y, 0.0f, 1.0f);
		m_MouseOrthoPos = inverseMat * m_MouseOrthoPos;
	}
}