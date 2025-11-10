#include "Render.h"

namespace Isle
{
    void Render::Start(Window* window)
    {
        m_Window = window;
        glfwSwapInterval(0);

        m_Pipeline = new Pipeline();
        m_Pipeline->Start();

        m_LastFrameTime = std::chrono::high_resolution_clock::now();
    }

    void Render::Destroy()
    {
        if (m_Pipeline)
        {
            m_Pipeline->Destroy();
            delete m_Pipeline;
            m_Pipeline = nullptr;
        }
    }

    void Render::Update()
    {

    }

    void Render::BeginFrame(const float* clear_color)
    {
        const float default_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        const float* color = clear_color ? clear_color : default_color;

        glClearColor(color[0], color[1], color[2], color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Stats.ResetPerFrame();
    }

    void Render::RenderFrame()
    {
        if (!m_Pipeline)
            return;

        auto renderStart = std::chrono::high_resolution_clock::now();

        m_Pipeline->Update();

        m_Stats.MeshCount = m_Pipeline->GetNumStaticMeshes();
        m_Stats.VertexCount = m_Pipeline->GetNumVertices();
        m_Stats.IndexCount = m_Pipeline->GetNumIndicies();
        m_Stats.LightCount = m_Pipeline->GetNumLights();
        m_Stats.MaterialCount = m_Pipeline->GetNumMaterials();
        m_Stats.TextureCount = m_Pipeline->GetNumTextures();

        auto renderEnd = std::chrono::high_resolution_clock::now();
        m_Stats.RenderTimeCPU =
            std::chrono::duration<double, std::milli>(renderEnd - renderStart).count();

        m_Stats.RenderFrameCount++;
        m_LastFrameTime = renderEnd;
    }

    void Render::EndFrame()
    {
        if (!m_Window && m_Window->m_Handle)
            glfwSwapBuffers(m_Window->m_Handle);
    }

    void Render::Reset()
    {
        glFinish();

        if (m_Pipeline)
        {
            m_Pipeline->Destroy();
            delete m_Pipeline;
        }

        m_Pipeline = new Pipeline();
        m_Pipeline->Start();
        m_Pipeline->Clear();
    }

    Ref<Texture> Render::GetOutputTexture()
    {
        return m_Pipeline ? m_Pipeline->GetFinalOutput() : nullptr;
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
