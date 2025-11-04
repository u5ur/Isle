// FrameBuffer.cpp
#include "FrameBuffer.h"
#include "../Texture/Texture.h"

namespace Isle
{
    FrameBuffer::FrameBuffer(int width, int height)
        : m_Width(width), m_Height(height)
    {
        Create();
    }

    FrameBuffer::~FrameBuffer()
    {
        Destroy();
    }

    void FrameBuffer::Create()
    {
        if (!m_Id)
            glGenFramebuffers(1, &m_Id);

        m_IsLoaded = true;
    }

    void FrameBuffer::Destroy()
    {
        if (m_Id)
        {
            glDeleteFramebuffers(1, &m_Id);
            m_Id = 0;
        }
        m_Attachments.clear();
        m_DrawBuffers.clear();
        m_IsLoaded = false;
        m_IsResident = false;
    }

    void FrameBuffer::Load()
    {
        if (!m_IsLoaded)
            Create();
    }

    void FrameBuffer::Bind(uint32_t)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        glViewport(0, 0, m_Width, m_Height);
        m_IsResident = true;
    }

    void FrameBuffer::Unbind(uint32_t)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_IsResident = false;
    }

    Texture* FrameBuffer::AddAttachment(ATTACHMENT_TYPE type, TEXTURE_FORMAT format, bool generateMipmaps)
    {
        if (!m_Id)
            Create();

        if (type == ATTACHMENT_TYPE::DEPTH)
            format = TEXTURE_FORMAT::DEPTH32F;
        else if (type == ATTACHMENT_TYPE::DEPTH_STENCIL)
            format = TEXTURE_FORMAT::DEPTH24_STENCIL8;

        auto* texture = new Texture();
        texture->Create(m_Width, m_Height, format, nullptr, generateMipmaps);

        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        GLenum attachment = ResolveAttachment(type, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->m_Id, 0);

        if (type != ATTACHMENT_TYPE::DEPTH && type != ATTACHMENT_TYPE::DEPTH_STENCIL)
        {
            GLenum colorAttachment = ResolveAttachment(type, static_cast<int>(m_DrawBuffers.size()));
            m_DrawBuffers.push_back(colorAttachment);

            if (!m_DrawBuffers.empty()) {
                glDrawBuffers(static_cast<GLsizei>(m_DrawBuffers.size()), m_DrawBuffers.data());
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_Attachments[type] = texture;
        return texture;
    }

    void FrameBuffer::AttachTexture(ATTACHMENT_TYPE type, Texture* texture, int attachmentIndex)
    {
        if (!texture || !m_Id) return;

        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        GLenum attachment = ResolveAttachment(type, attachmentIndex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->m_Id, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_Attachments[type] = texture;

        if (type != ATTACHMENT_TYPE::DEPTH && type != ATTACHMENT_TYPE::DEPTH_STENCIL)
        {
            m_DrawBuffers.push_back(attachment);
        }
    }

    void FrameBuffer::AttachDepthTexture(Texture* texture)
    {
        AttachTexture(ATTACHMENT_TYPE::DEPTH, texture);
    }

    void FrameBuffer::AttachDepthStencilTexture(Texture* texture)
    {
        AttachTexture(ATTACHMENT_TYPE::DEPTH_STENCIL, texture);
    }

    Texture* FrameBuffer::GetAttachment(ATTACHMENT_TYPE type)
    {
        auto it = m_Attachments.find(type);
        return (it != m_Attachments.end()) ? it->second : nullptr;
    }

    void FrameBuffer::SetDrawBuffers(const std::vector<ATTACHMENT_TYPE>& targets)
    {
        m_DrawBuffers.clear();

        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

        for (int i = 0; i < targets.size(); i++) {
            ATTACHMENT_TYPE type = targets[i];
            if (type != ATTACHMENT_TYPE::DEPTH && type != ATTACHMENT_TYPE::DEPTH_STENCIL) {
                GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
                m_DrawBuffers.push_back(attachment);

                Texture* tex = GetAttachment(type);
                if (tex) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->m_Id, 0);
                }
            }
        }

        if (!m_DrawBuffers.empty()) {
            glDrawBuffers(static_cast<GLsizei>(m_DrawBuffers.size()), m_DrawBuffers.data());
        }
        else {
            glDrawBuffer(GL_NONE);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool FrameBuffer::CheckStatus()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            ISLE_ERROR("Framebuffer incomplete! Status: 0x%x\n", status);
            return false;
        }
        return true;
    }

    void FrameBuffer::Clear(const glm::vec4& color, float depth, int stencil)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        glClearColor(color.r, color.g, color.b, color.a);
        glClearDepth(depth);
        glClearStencil(stencil);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }


    void FrameBuffer::ClearColor(const glm::vec4& color)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::ClearDepth(float depth)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        glClearDepth(depth);
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Resize(int width, int height)
    {
        m_Width = width;
        m_Height = height;

        for (auto& [type, texture] : m_Attachments)
        {
            if (texture)
                texture->Resize(width, height);
        }
    }

    void FrameBuffer::BlitTo(FrameBuffer* target, GLbitfield mask, GLenum filter)
    {
        GLuint targetId = target ? target->m_Id : 0;
        int targetWidth = target ? target->m_Width : m_Width;
        int targetHeight = target ? target->m_Height : m_Height;

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Id);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetId);
        glBlitFramebuffer(0, 0, m_Width, m_Height,
            0, 0, targetWidth, targetHeight,
            mask, filter);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::BlitToTexture(Texture* targetTexture, GLbitfield mask, GLenum filter)
    {
        if (!targetTexture) 
            return;

        GLuint tempFBO;
        glGenFramebuffers(1, &tempFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, tempFBO);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, targetTexture->m_Id, 0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Id);
        glBlitFramebuffer(0, 0, m_Width, m_Height,
            0, 0, targetTexture->m_Width, targetTexture->m_Height,
            mask, filter);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &tempFBO);
    }

    void FrameBuffer::SetDebugLabel(const std::string& name)
    {
        if (glObjectLabel)
            glObjectLabel(GL_FRAMEBUFFER, m_Id, -1, name.c_str());
    }

    GLenum FrameBuffer::ResolveAttachment(ATTACHMENT_TYPE type, int index)
    {
        switch (type)
        {
            case ATTACHMENT_TYPE::COLOR:
            case ATTACHMENT_TYPE::NORMAL:
            case ATTACHMENT_TYPE::EMISSIVE:
            case ATTACHMENT_TYPE::MATERIAL:
            case ATTACHMENT_TYPE::VELOCITY:
            case ATTACHMENT_TYPE::LIGHTING:
            case ATTACHMENT_TYPE::INDIRECT:
            case ATTACHMENT_TYPE::SPECULAR:
            case ATTACHMENT_TYPE::REFLECTION:
            case ATTACHMENT_TYPE::RADIANCE:
            case ATTACHMENT_TYPE::SCENE:
            case ATTACHMENT_TYPE::HDR:
            case ATTACHMENT_TYPE::FINAL:
                return GL_COLOR_ATTACHMENT0 + index;

            case ATTACHMENT_TYPE::DEPTH:
                return GL_DEPTH_ATTACHMENT;

            case ATTACHMENT_TYPE::DEPTH_STENCIL:
                return GL_DEPTH_STENCIL_ATTACHMENT;

            default:
                return GL_COLOR_ATTACHMENT0;
        }
    }
}