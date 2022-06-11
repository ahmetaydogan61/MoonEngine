#include "mpch.h"
#include "Window.h"
#include "Event/Events.h"

#include "stb_image.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace MoonEngine
{
	GLFWwindow* Window::m_GLWindow;
	std::string Window::m_Name;
	uint32_t Window::m_Width;
	uint32_t Window::m_Height;
	bool Window::m_Vsync = true;
	std::function<void(Event&)> Window::m_EventCallback;

	int Window::Create(std::string name, uint32_t width, uint32_t height, bool fullscreen)
	{
		m_Name = name;
		m_Width = width;
		m_Height = height;

		if (!glfwInit())
			return -1;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		glfwWindowHint(GLFW_MAXIMIZED, fullscreen);

		m_GLWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);


		if (!m_GLWindow)
		{
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(m_GLWindow);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status)
		{
			DebugErr("Failed To Initialize Glad");
			return -1;
		}

		//Center the monitor
		if (!fullscreen)
		{
			const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowPos(m_GLWindow, (vidmode->width - m_Width) / 2, (vidmode->height - m_Height) / 2);
		}

		//Callbacks
		glfwSetWindowSizeCallback(m_GLWindow, [](GLFWwindow* window, int width, int height)
		{
			m_Width = width;
			m_Height = height;
		});

		glfwSetFramebufferSizeCallback(m_GLWindow, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});

		glfwSetScrollCallback(m_GLWindow, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			MouseScrollEvent e{ (float)xoffset, (float)yoffset };
			m_EventCallback(e);
		});

		glfwSwapInterval(m_Vsync);
		return 1;
	}

	void Window::Update()
	{
		glfwSwapBuffers(m_GLWindow);
		glfwPollEvents();
	}

	void Window::SetEventCallback(const std::function<void(Event&)>& e)
	{
		m_EventCallback = e;
	}

	bool Window::IsRunning()
	{
		return !glfwWindowShouldClose(m_GLWindow);
	}

	void Window::SetVsync(bool state)
	{
		m_Vsync = state;
		glfwSwapInterval(m_Vsync);
	}

	void Window::SetIcon(const std::string& path)
	{
		GLFWimage images[1];
		images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(m_GLWindow, 1, images);
		stbi_image_free(images[0].pixels);
	}

	void Window::Destroy()
	{
		glfwTerminate();
	}
}