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
        if (!m_Id) return;
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        m_IsResident = true;
    }

    void FrameBuffer::Unbind(uint32_t)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_IsResident = false;
    }

    Texture* FrameBuffer::AddAttachment(RENDER_TARGET_TYPE type,
        TEXTURE_FORMAT format,
        bool generateMipmaps)
    {
        if (!m_Id)
            Create();

        auto* texture = new Texture();
        texture->Create(m_Width, m_Height, format, nullptr, generateMipmaps);

        AttachTexture(type, texture);

        if (type == RENDER_TARGET_TYPE::DEPTH)
            texture->m_Format = TEXTURE_FORMAT::DEPTH32F;
        else if (type == RENDER_TARGET_TYPE::DEPTH_STENCIL)
            texture->m_Format = TEXTURE_FORMAT::DEPTH24_STENCIL8;

        m_Attachments[type] = texture;

        if (type != RENDER_TARGET_TYPE::DEPTH && type != RENDER_TARGET_TYPE::DEPTH_STENCIL)
        {
            GLenum attachment = ResolveAttachment(type, static_cast<int>(m_DrawBuffers.size()));
            m_DrawBuffers.push_back(attachment);
            glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
            glDrawBuffers(static_cast<GLsizei>(m_DrawBuffers.size()), m_DrawBuffers.data());
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        return texture;
    }


    void FrameBuffer::AttachTexture(RENDER_TARGET_TYPE type, Texture* texture, int attachmentIndex)
    {
        if (!texture || !m_Id) return;

        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        GLenum attachment = ResolveAttachment(type, attachmentIndex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->m_Id, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_Attachments[type] = texture;

        if (type != RENDER_TARGET_TYPE::DEPTH && type != RENDER_TARGET_TYPE::DEPTH_STENCIL)
        {
            m_DrawBuffers.push_back(attachment);
        }
    }

    void FrameBuffer::AttachDepthTexture(Texture* texture)
    {
        AttachTexture(RENDER_TARGET_TYPE::DEPTH, texture);
    }

    void FrameBuffer::AttachDepthStencilTexture(Texture* texture)
    {
        AttachTexture(RENDER_TARGET_TYPE::DEPTH_STENCIL, texture);
    }

    Texture* FrameBuffer::GetAttachment(RENDER_TARGET_TYPE type)
    {
        auto it = m_Attachments.find(type);
        return (it != m_Attachments.end()) ? it->second : nullptr;
    }

    void FrameBuffer::SetDrawBuffers(const std::vector<RENDER_TARGET_TYPE>& targets)
    {
        m_DrawBuffers.clear();
        for (const auto& target : targets)
        {
            m_DrawBuffers.push_back(ResolveAttachment(target));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        if (!m_DrawBuffers.empty())
            glDrawBuffers(static_cast<GLsizei>(m_DrawBuffers.size()), m_DrawBuffers.data());
        else
            glDrawBuffer(GL_NONE);
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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetId);
        glBlitFramebuffer(0, 0, m_Width, m_Height,
                         0, 0, targetWidth, targetHeight,
                         mask, filter);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::SetDebugLabel(const std::string& name)
    {
        if (glObjectLabel)
            glObjectLabel(GL_FRAMEBUFFER, m_Id, -1, name.c_str());
    }

    GLenum FrameBuffer::ResolveAttachment(RENDER_TARGET_TYPE type, int index)
    {
        switch (type)
        {
            case RENDER_TARGET_TYPE::COLOR:
            case RENDER_TARGET_TYPE::NORMAL:
            case RENDER_TARGET_TYPE::EMISSIVE:
            case RENDER_TARGET_TYPE::MATERIAL:
            case RENDER_TARGET_TYPE::VELOCITY:
            case RENDER_TARGET_TYPE::LIGHTING:
            case RENDER_TARGET_TYPE::INDIRECT:
            case RENDER_TARGET_TYPE::SPECULAR:
            case RENDER_TARGET_TYPE::REFLECTION:
            case RENDER_TARGET_TYPE::RADIANCE:
            case RENDER_TARGET_TYPE::SCENE:
            case RENDER_TARGET_TYPE::HDR:
            case RENDER_TARGET_TYPE::FINAL:
                return GL_COLOR_ATTACHMENT0 + index;

            case RENDER_TARGET_TYPE::DEPTH:
                return GL_DEPTH_ATTACHMENT;

            case RENDER_TARGET_TYPE::DEPTH_STENCIL:
                return GL_DEPTH_STENCIL_ATTACHMENT;

            default:
                return GL_COLOR_ATTACHMENT0;
        }
    }
}