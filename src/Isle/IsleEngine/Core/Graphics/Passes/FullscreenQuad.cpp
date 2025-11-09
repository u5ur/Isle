// FullscreenQuad.cpp
#include "FullscreenQuad.h"

namespace Isle
{
    FullscreenQuad::FullscreenQuad()
    {
        Create();
    }

    FullscreenQuad::~FullscreenQuad()
    {
        Destroy();
    }

    void FullscreenQuad::Create()
    {
        float vertices[] =
        {
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f
        };

        unsigned int indices[] =
        {
            0, 1, 2,
            0, 2, 3
        };

        m_VertexBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::VERTEX, sizeof(vertices), vertices);
        m_VertexBuffer->AddVertexAttribute(0, 3, GL_FLOAT, GL_FALSE,
            5 * sizeof(float), (void*)0);
        m_VertexBuffer->AddVertexAttribute(1, 2, GL_FLOAT, GL_FALSE,
            5 * sizeof(float), (void*)(3 * sizeof(float)));

        m_IndexBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::INDEX, sizeof(indices), indices);
        m_VertexBuffer->SetIndexBuffer(m_IndexBuffer.Get());
        m_VertexBuffer->SetupVertexAttributes();
    }

    void FullscreenQuad::Destroy()
    {
    }

    void FullscreenQuad::Draw()
    {
        if (!m_VertexBuffer || !m_IndexBuffer)
            return;

        m_VertexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        m_VertexBuffer->Unbind();
    }
}