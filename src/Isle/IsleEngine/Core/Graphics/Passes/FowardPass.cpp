// FowardPass.cpp
#include "FowardPass.h"

namespace Isle
{
    void ForwardPass::Start()
    {
        m_Shader = new Shader();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "..\\Resources\\Shaders\\Forward.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "..\\Resources\\Shaders\\Forward.vert");
        m_Shader->Link();

        m_FrameBuffer = new FrameBuffer(1920, 1080);
        ISLE_LOG("FBO created with ID: %d\n", m_FrameBuffer->m_Id);

        m_FrameBuffer->AddAttachment(RENDER_TARGET_TYPE::COLOR, TEXTURE_FORMAT::RGBA8);
        m_FrameBuffer->AddAttachment(RENDER_TARGET_TYPE::DEPTH, TEXTURE_FORMAT::DEPTH32F);

        ISLE_LOG("FBO after attachments ID: %d\n", m_FrameBuffer->m_Id);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("ForwardPass framebuffer incomplete!\n");
        }

        m_PipelineState = new PipelineState();
        m_PipelineState->SetDepthTest(true);
        m_PipelineState->SetDepthWrite(true);
        m_PipelineState->SetCullEnabled(false);
    }

    void ForwardPass::Update()
    {
    }

    void ForwardPass::Bind()
    {
        m_FrameBuffer->Bind();
        m_FrameBuffer->Clear(glm::vec4(0, 0, 0, 0));

        m_Shader->Bind();
        m_PipelineState->Bind();
    }

    void ForwardPass::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void ForwardPass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }
}
