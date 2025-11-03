// Render.cpp
#include "Render.h"

namespace Isle
{
    void Render::Start()
    {
        m_Pipeline = new Pipeline();
        m_Pipeline->Start();
    }

    void Render::Update()
    {
        m_Pipeline->Update();
    }

    void Render::Destroy()
    {
        m_Pipeline->Destroy();
    }

    const Pipeline* Render::GetPipeline()
    {
        return m_Pipeline;
    }
}