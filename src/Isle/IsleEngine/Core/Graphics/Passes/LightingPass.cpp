#include "LightingPass.h"

namespace Isle
{
    void LightingPass::Start()
    {
        m_FrameBuffer = New<FrameBuffer>(1920, 1080);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::COLOR, TEXTURE_FORMAT::RGBA16F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::DEPTH, TEXTURE_FORMAT::DEPTH24_STENCIL8);

        std::vector<ATTACHMENT_TYPE> drawTargets = {
            ATTACHMENT_TYPE::COLOR
        };
        m_FrameBuffer->SetDrawBuffers(drawTargets);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("LightingPass framebuffer incomplete!\n");
        }

        m_Shader = New<Shader>();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "Resources\\Shaders\\Lighting.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "Resources\\Shaders\\Lighting.vert");
        m_Shader->Link();

        m_PipelineState = New<PipelineState>();

        m_PipelineState->SetDepthTest(false);
        m_PipelineState->SetDepthWrite(false);
        m_PipelineState->SetBlendEnabled(false);
        m_PipelineState->SetCullEnabled(false);

    }

    void LightingPass::Update()
    {
    }

    void LightingPass::Bind()
    {
        if (m_FrameBuffer)
        {
            m_FrameBuffer->Bind();
            m_FrameBuffer->Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        if (m_Shader)
            m_Shader->Bind();

        if (m_PipelineState)
            m_PipelineState->Bind();
    }

    void LightingPass::Unbind()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Unbind();
    }

    void LightingPass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }

    Ref<Texture> LightingPass::GetOutputTexture()
    {
        if (m_FrameBuffer)
        {
            return m_FrameBuffer->GetAttachment(ATTACHMENT_TYPE::COLOR);
        }
        return nullptr;
    }

    void LightingPass::BlitToScreen()
    {
        if (m_FrameBuffer)
        {
            m_FrameBuffer->BlitTo(nullptr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }
}