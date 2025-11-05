// FowardPass.cpp
#include "GeometryPass.h"

namespace Isle
{
    void GeometryPass::Start()
    {
        m_FrameBuffer = New<FrameBuffer>(1920, 1080);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::COLOR, TEXTURE_FORMAT::RGBA16F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::NORMAL, TEXTURE_FORMAT::RGBA16F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::POSITION, TEXTURE_FORMAT::RGBA32F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::MATERIAL, TEXTURE_FORMAT::RGBA16F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::EMISSIVE, TEXTURE_FORMAT::RGBA16F);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::SELECTION, TEXTURE_FORMAT::R8);
        m_FrameBuffer->AddAttachment(ATTACHMENT_TYPE::DEPTH, TEXTURE_FORMAT::DEPTH32F);

        std::vector<ATTACHMENT_TYPE> drawTargets = {
            ATTACHMENT_TYPE::COLOR,
            ATTACHMENT_TYPE::NORMAL,
            ATTACHMENT_TYPE::POSITION,
            ATTACHMENT_TYPE::MATERIAL,
            ATTACHMENT_TYPE::EMISSIVE,
            ATTACHMENT_TYPE::SELECTION
        };
        m_FrameBuffer->SetDrawBuffers(drawTargets);

        if (!m_FrameBuffer->CheckStatus())
        {
            ISLE_ERROR("ForwardPass framebuffer incomplete!\n");
        }

        m_Shader = New<Shader>();
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "..\\Resources\\Shaders\\Geometry.frag");
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "..\\Resources\\Shaders\\Geometry.vert");
        m_Shader->Link();

        m_PipelineState = New<PipelineState>();
        m_PipelineState->SetDepthTest(true);
        m_PipelineState->SetDepthWrite(true);
        m_PipelineState->SetDepthFunc(COMPARE_FUNC::LESS);
        m_PipelineState->SetCullEnabled(true);
        m_PipelineState->SetCullFace(CULL_MODE::BACK);
    }

    void GeometryPass::Update()
    {
    }

    void GeometryPass::Bind()
    {
        m_FrameBuffer->Bind();

        m_Shader->Bind();
        m_PipelineState->Bind();
        m_FrameBuffer->Clear();
    }

    void GeometryPass::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void GeometryPass::Destroy()
    {
        if (m_FrameBuffer)
            m_FrameBuffer->Destroy();
    }
}
