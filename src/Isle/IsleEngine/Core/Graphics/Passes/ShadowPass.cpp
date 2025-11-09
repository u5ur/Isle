// ShadowPass.cpp
#include "ShadowPass.h"

namespace Isle
{
    void ShadowPass::Start()
    {
        m_FrameBuffer = New<FrameBuffer>(m_Size, m_Size);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::SHADOW_MAP);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("ForwardPass framebuffer incomplete!\n");
        }

        m_Shader = New<Shader>();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "Resources\\Shaders\\Shadow.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "Resources\\Shaders\\Shadow.vert");
        m_Shader->Link();

        m_PipelineState = New<PipelineState>();
		m_PipelineState->SetDepthTest(true);
		m_PipelineState->SetDepthWrite(true);
		m_PipelineState->SetCullEnabled(true);
		m_PipelineState->SetCullFace(CULL_MODE::FRONT);
    }

    void ShadowPass::Update()
    {
    }

    void ShadowPass::Bind()
    {
        m_FrameBuffer->Bind();
        m_PipelineState->Bind();
        m_FrameBuffer->Clear();
        m_Shader->Bind();
    }

    void ShadowPass::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void ShadowPass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }

    void ShadowPass::SetSize(int size)
    {
        m_Size = size;
    }

    int ShadowPass::GetSize()
    {
        return m_Size;
    }
}
