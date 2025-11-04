// Render.cpp
#include "Render.h"

namespace Isle
{
    void Render::Start()
    {
        m_Window = new Window();
        m_Window->Start();

        m_Pipeline = new Pipeline();
        m_Pipeline->Start();

        if (m_ImGui)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui_ImplGlfw_InitForOpenGL(m_Window->m_Handle, true);
            ImGui_ImplOpenGL3_Init("#version 460");
            ImGui::StyleColorsDark();

            ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
        }
    }

    void Render::Update()
    {
        m_Pipeline->Update();
    }

    void Render::Destroy()
    {
        m_Pipeline->Destroy();
    }

    void Render::BeginImGuiFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void Render::RenderImGuiFrame()
    {
        ImGui::Render();
    }

    void Render::EndImGuiFrame()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Render::BeginFrame(const float* clear_color)
    {
		float default_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		const float* color = clear_color ? clear_color : default_color;

		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();

        if (m_ImGui)
        {
            BeginImGuiFrame();
        }
	}

	void Render::RenderFrame()
	{
		m_Pipeline->Update();

        if (m_ImGui)
        {
            RenderImGuiFrame();
        }
	}

    void Render::EndFrame()
    {
        glfwSwapBuffers(m_Window->m_Handle);

        if (m_ImGui)
        {
            EndImGuiFrame();
        }
    }

    Pipeline* Render::GetPipeline()
    {
        return m_Pipeline;
    }

    Window* Render::GetWindow()
    {
        return m_Window;
    }
}
