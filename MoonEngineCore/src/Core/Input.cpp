#include "mpch.h"
#include "Core/Input.h"
#include "Core/Window.h"

#include <GLFW/glfw3.h>

namespace MoonEngine
{
	bool Input::GetKey(Keycode key)
	{
		return glfwGetKey(Window::GetWindow(), (int)key);
	}

	float Input::ScreenX()
	{
		double mouseX;
		glfwGetCursorPos(Window::GetWindow(), &mouseX, nullptr);
		m_ScreenPos.x = (float)mouseX;
		return m_ScreenPos.x;
	}

	float Input::ScreenY()
	{
		double mouseY;
		glfwGetCursorPos(Window::GetWindow(), nullptr, &mouseY);
		m_ScreenPos.y = (float)mouseY;
		return m_ScreenPos.y;
	}

	float Input::GetX()
	{
		return m_MouseOrthoPos.x;
	}

	float Input::GetY()
	{
		return m_MouseOrthoPos.y;
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
		float x = (2.0f * ((m_ScreenPos.x - viewPortPos.x) / (viewPortSize.x - 0))) - 1.0f;
		float y = (2.0f * (1.0f - ((m_ScreenPos.y - viewPortPos.y) / (viewPortSize.y - 0)))) - 1.0f;
		m_MouseOrthoPos = glm::vec4(x, y, 0.0f, 1.0f);
		m_MouseOrthoPos = inverseMat * m_MouseOrthoPos;
	}

	void Input::SetCursorMode(CursorMode mode)
	{
		switch (mode)
		{
			case CursorMode::NONE:
				glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case CursorMode::LOCKED:
				glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
			default:
				break;
		}
	}
}