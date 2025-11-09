// SelectionPass.cpp
#include "SelectionPass.h"

namespace Isle
{
    void SelectionPass::Start()
    {
        m_FrameBuffer = New<FrameBuffer>(1920, 1080);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::SELECTION, TEXTURE_FORMAT::R8);

        std::vector<ATTACHMENT_TYPE> drawTargets = {
            ATTACHMENT_TYPE::SELECTION
        };
        m_FrameBuffer->SetDrawBuffers(drawTargets);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("CompositePass framebuffer incomplete!\n");
        }

        m_Shader = New<Shader>();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "Resources\\Shaders\\Selection.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "Resources\\Shaders\\Selection.vert");
        m_Shader->Link();

        m_PipelineState = New<PipelineState>();
        m_PipelineState->SetDepthTest(false);
        m_PipelineState->SetDepthWrite(false);
        m_PipelineState->SetCullEnabled(false);
    }

    void SelectionPass::Update()
    {
    }

    void SelectionPass::Bind()
    {
        m_FrameBuffer->Bind();
        m_FrameBuffer->Clear();

        m_Shader->Bind();
        m_PipelineState->Bind();
    }

    void SelectionPass::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void SelectionPass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }
}