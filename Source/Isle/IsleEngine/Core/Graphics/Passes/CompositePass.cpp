// CompositePass.cpp
#include "CompositePass.h"

namespace Isle
{
    void CompositePass::Start()
    {
        m_FrameBuffer = New<FrameBuffer>(1920, 1080);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::COLOR, TEXTURE_FORMAT::RGBA16F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::DEPTH, TEXTURE_FORMAT::DEPTH32F);

        std::vector<ATTACHMENT_TYPE> drawTargets = {
            ATTACHMENT_TYPE::COLOR
        };
        m_FrameBuffer->SetDrawBuffers(drawTargets);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("CompositePass framebuffer incomplete!\n");
        }

        m_Shader = New<Shader>();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "Resources\\Shaders\\Composite.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "Resources\\Shaders\\Composite.vert");
        m_Shader->Link();

        m_PipelineState = New<PipelineState>();
        m_PipelineState->SetDepthTest(false);
        m_PipelineState->SetDepthWrite(false);
        m_PipelineState->SetCullEnabled(false);
        m_PipelineState->SetBlendEnabled(false);
    }

    void CompositePass::Update()
    {
    }

    void CompositePass::Bind()
    {
        m_FrameBuffer->Bind();
        m_FrameBuffer->Clear();
        m_Shader->Bind();
        m_PipelineState->Bind();
    }

    void CompositePass::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void CompositePass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }

    Ref<Texture> CompositePass::GetOutputTexture()
    {
        if (m_FrameBuffer)
        {
            return m_FrameBuffer->GetAttachment(ATTACHMENT_TYPE::COLOR);
        }
        return nullptr;
    }

    void CompositePass::BlitToScreen()
    {
        if (m_FrameBuffer)
        {
            m_FrameBuffer->BlitTo(nullptr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }
}