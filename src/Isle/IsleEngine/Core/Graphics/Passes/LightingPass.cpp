// LightingPass.cpp
#include "LightingPass.h"

namespace Isle
{
    void LightingPass::Start()
    {
        m_FrameBuffer = new FrameBuffer(1920, 1080);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::COLOR, TEXTURE_FORMAT::RGBA16F);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("ForwardPass framebuffer incomplete!\n");
        }

        m_Shader = new Shader();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "..\\Resources\\Shaders\\Lighting.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "..\\Resources\\Shaders\\Lighting.vert");
        m_Shader->Link();

        m_PipelineState = new PipelineState();

        m_PipelineState->SetDepthTest(false);
        m_PipelineState->SetDepthWrite(false);
        m_PipelineState->SetBlendEnabled(true);
        m_PipelineState->SetBlendSrc(BLEND_FACTOR::ONE);
        m_PipelineState->SetBlendDst(BLEND_FACTOR::ONE);
        m_PipelineState->SetCullEnabled(false);
    }

    void LightingPass::Update()
    {
    }

    void LightingPass::Bind()
    {
        m_FrameBuffer->Bind();
        m_FrameBuffer->Clear();

        m_Shader->Bind();

        m_PipelineState->Bind();
    }

    void LightingPass::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void LightingPass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }
}
