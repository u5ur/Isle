#include "Render.h"

namespace Isle
{
    void Render::Start(Window* window)
    {
        m_Window = window;
        m_Pipeline = new Pipeline();
        m_Pipeline->Start();
    }

    void Render::Update()
    {
        if (m_Pipeline)
            m_Pipeline->Update();
    }

    void Render::Destroy()
    {
    }

    void Render::BeginFrame(const float* clear_color)
    {
        float default_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        const float* color = clear_color ? clear_color : default_color;

        glClearColor(color[0], color[1], color[2], color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render::RenderFrame()
    {
        if (m_Pipeline)
            m_Pipeline->Update();
    }

    void Render::EndFrame()
    {
        if (!m_IsEditorMode && m_Window && m_Window->m_Handle)
            glfwSwapBuffers(m_Window->m_Handle);
    }

    void Render::Reset()
    {
        glFinish();

        if (m_Pipeline) {
            m_Pipeline->Destroy();
            delete m_Pipeline;
            m_Pipeline = nullptr;
        }

        m_Pipeline = new Pipeline();
        m_Pipeline->Start();
        m_Pipeline->Clear();
    }

    Ref<Texture> Render::GetOutputTexture()
    {
        if (m_Pipeline)
        {
            return m_Pipeline->GetFinalOutput();
        }

        return nullptr;
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