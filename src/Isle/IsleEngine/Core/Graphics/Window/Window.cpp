//Window.cpp
#include "Window.h"

namespace Isle
{
	static constexpr int MAJOR_VERSION = 4;
	static constexpr int MINOR_VERSION = 6;
	static const auto WINDOW_TITLE = "Isle";

	void Window::Start()
	{
		if (!glfwInit())
		{
			ISLE_ERROR("Failed to initialize glfw!\n");
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Handle = glfwCreateWindow(m_Width, m_Height, WINDOW_TITLE, nullptr, nullptr);
		if (!m_Handle)
		{
			ISLE_ERROR("Failed to create '%s' window!\n", WINDOW_TITLE);
			return;
		}

		glfwMakeContextCurrent(m_Handle);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			ISLE_ERROR("Failed to initialize glew!\n");
			return;
		}
	}

	void Window::Update()
	{
	}

	void Window::Destroy()
	{
	}
}
