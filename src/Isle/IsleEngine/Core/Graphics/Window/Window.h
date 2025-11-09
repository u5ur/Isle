#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
	class Window : public Component
	{
	public:
		GLFWwindow* m_Handle;
		int m_Width = 1920;
		int m_Height = 1080;
	public:
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;

		void PollEvents();
		void SwapBuffers();
		bool ShouldClose();

		glm::ivec2 GetFramebufferSize();
		void Clear(const glm::vec4& color);
	};
}