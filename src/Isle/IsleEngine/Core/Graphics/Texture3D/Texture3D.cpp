// Texture3D.cpp
#include "Texture3D.h"

namespace Isle
{
    Texture3D::~Texture3D()
    {
        Destroy();
    }

    void Texture3D::Create(int width, int height, int depth, TEXTURE3D_FORMAT format,
                          const void* data, bool generateMipmaps)
    {
        m_Width = width;
        m_Height = height;
        m_Depth = depth;
        m_Format = format;
        m_GenerateMipmaps = generateMipmaps;

        if (!m_Id)
            glGenTextures(1, &m_Id);

        glBindTexture(GL_TEXTURE_3D, m_Id);

        GLenum internalFormat = ResolveInternalFormat(format);
        GLenum dataFormat = ResolveFormat(format);
        GLenum dataType = ResolveDataType(format);

        glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0,
                    dataFormat, dataType, data);

        SetMinFilter(m_MinFilter);
        SetMagFilter(m_MagFilter);
        SetWrap(m_WrapS, m_WrapT, m_WrapR);

        if (generateMipmaps && data)
            GenerateMipmaps();

        glBindTexture(GL_TEXTURE_3D, 0);

        m_IsLoaded = true;
        m_SizeInBytes = width * height * depth * 4;
    }

    void Texture3D::Destroy()
    {
        if (m_Id)
        {
            glDeleteTextures(1, &m_Id);
            m_Id = 0;
        }
        m_IsLoaded = false;
        m_IsResident = false;
        m_SizeInBytes = 0;
        m_Slot = -1;
        m_ImageSlot = -1;
    }

    void Texture3D::Load()
    {
        if (m_IsLoaded) return;
    }

    void Texture3D::Bind(uint32_t slot)
    {
        if (!m_Id) return;
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_3D, m_Id);
        m_Slot = slot;
        m_IsResident = true;
    }

    void Texture3D::Unbind(uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_3D, 0);
        m_IsResident = false;
        m_Slot = -1;
    }

    void Texture3D::BindAsImage(uint32_t slot, GLenum access)
    {
        if (!m_Id) return;
        GLenum format = ResolveInternalFormat(m_Format);
        glBindImageTexture(slot, m_Id, 0, GL_TRUE, 0, access, format);
        m_ImageSlot = slot;
    }

    void Texture3D::UnbindAsImage(uint32_t slot)
    {
        glBindImageTexture(slot, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
        m_ImageSlot = -1;
    }

    void Texture3D::SetMinFilter(TEXTURE3D_FILTER filter)
    {
        m_MinFilter = filter;
        glBindTexture(GL_TEXTURE_3D, m_Id);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, ResolveFilter(filter));
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::SetMagFilter(TEXTURE3D_FILTER filter)
    {
        m_MagFilter = filter;
        glBindTexture(GL_TEXTURE_3D, m_Id);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, ResolveFilter(filter));
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::SetWrap(TEXTURE3D_WRAP wrapS, TEXTURE3D_WRAP wrapT, TEXTURE3D_WRAP wrapR)
    {
        m_WrapS = wrapS;
        m_WrapT = wrapT;
        m_WrapR = wrapR;
        glBindTexture(GL_TEXTURE_3D, m_Id);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, ResolveWrap(wrapS));
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, ResolveWrap(wrapT));
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, ResolveWrap(wrapR));
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::SetBorderColor(const glm::vec4& color)
    {
        glBindTexture(GL_TEXTURE_3D, m_Id);
        glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::Upload(const void* data, int level)
    {
        glBindTexture(GL_TEXTURE_3D, m_Id);
        GLenum format = ResolveFormat(m_Format);
        GLenum type = ResolveDataType(m_Format);
        glTexSubImage3D(GL_TEXTURE_3D, level, 0, 0, 0, m_Width, m_Height, m_Depth,
                       format, type, data);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::Download(void* outData, int level) const
    {
        glBindTexture(GL_TEXTURE_3D, m_Id);
        GLenum format = ResolveFormat(m_Format);
        GLenum type = ResolveDataType(m_Format);
        glGetTexImage(GL_TEXTURE_3D, level, format, type, outData);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::GenerateMipmaps()
    {
        glBindTexture(GL_TEXTURE_3D, m_Id);
        glGenerateMipmap(GL_TEXTURE_3D);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::Clear(const glm::vec4& clearColor)
    {
        glBindTexture(GL_TEXTURE_3D, m_Id);
        glClearTexImage(m_Id, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(clearColor));
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::Resize(int width, int height, int depth)
    {
        m_Width = width;
        m_Height = height;
        m_Depth = depth;

        glBindTexture(GL_TEXTURE_3D, m_Id);
        GLenum internalFormat = ResolveInternalFormat(m_Format);
        GLenum format = ResolveFormat(m_Format);
        GLenum type = ResolveDataType(m_Format);
        glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0,
                    format, type, nullptr);
        glBindTexture(GL_TEXTURE_3D, 0);

        m_SizeInBytes = width * height * depth * 4;
    }

    void Texture3D::SetDebugLabel(const std::string& name)
    {
        if (glObjectLabel)
            glObjectLabel(GL_TEXTURE, m_Id, -1, name.c_str());
    }

    GLenum Texture3D::ResolveInternalFormat(TEXTURE3D_FORMAT format)
    {
        switch (format)
        {
            case TEXTURE3D_FORMAT::R8: return GL_R8;
            case TEXTURE3D_FORMAT::RG8: return GL_RG8;
            case TEXTURE3D_FORMAT::RGB8: return GL_RGB8;
            case TEXTURE3D_FORMAT::RGBA8: return GL_RGBA8;
            case TEXTURE3D_FORMAT::R16F: return GL_R16F;
            case TEXTURE3D_FORMAT::RG16F: return GL_RG16F;
            case TEXTURE3D_FORMAT::RGB16F: return GL_RGB16F;
            case TEXTURE3D_FORMAT::RGBA16F: return GL_RGBA16F;
            case TEXTURE3D_FORMAT::R32F: return GL_R32F;
            case TEXTURE3D_FORMAT::RG32F: return GL_RG32F;
            case TEXTURE3D_FORMAT::RGB32F: return GL_RGB32F;
            case TEXTURE3D_FORMAT::RGBA32F: return GL_RGBA32F;
            case TEXTURE3D_FORMAT::R32UI: return GL_R32UI;
            case TEXTURE3D_FORMAT::RG32UI: return GL_RG32UI;
            case TEXTURE3D_FORMAT::RGB32UI: return GL_RGB32UI;
            case TEXTURE3D_FORMAT::RGBA32UI: return GL_RGBA32UI;
            default: return GL_RGBA8;
        }
    }

    GLenum Texture3D::ResolveFormat(TEXTURE3D_FORMAT format)
    {
        switch (format)
        {
            case TEXTURE3D_FORMAT::R8:
            case TEXTURE3D_FORMAT::R16F:
            case TEXTURE3D_FORMAT::R32F: return GL_RED;

            case TEXTURE3D_FORMAT::RG8:
            case TEXTURE3D_FORMAT::RG16F:
            case TEXTURE3D_FORMAT::RG32F: return GL_RG;

            case TEXTURE3D_FORMAT::RGB8:
            case TEXTURE3D_FORMAT::RGB16F:
            case TEXTURE3D_FORMAT::RGB32F: return GL_RGB;

            case TEXTURE3D_FORMAT::RGBA8:
            case TEXTURE3D_FORMAT::RGBA16F:
            case TEXTURE3D_FORMAT::RGBA32F: return GL_RGBA;

            case TEXTURE3D_FORMAT::R32UI: return GL_RED_INTEGER;
            case TEXTURE3D_FORMAT::RG32UI: return GL_RG_INTEGER;
            case TEXTURE3D_FORMAT::RGB32UI: return GL_RGB_INTEGER;
            case TEXTURE3D_FORMAT::RGBA32UI: return GL_RGBA_INTEGER;

            default: return GL_RGBA;
        }
    }

    GLenum Texture3D::ResolveDataType(TEXTURE3D_FORMAT format)
    {
        switch (format)
        {
            case TEXTURE3D_FORMAT::R8:
            case TEXTURE3D_FORMAT::RG8:
            case TEXTURE3D_FORMAT::RGB8:
            case TEXTURE3D_FORMAT::RGBA8: return GL_UNSIGNED_BYTE;

            case TEXTURE3D_FORMAT::R16F:
            case TEXTURE3D_FORMAT::RG16F:
            case TEXTURE3D_FORMAT::RGB16F:
            case TEXTURE3D_FORMAT::RGBA16F: return GL_HALF_FLOAT;

            case TEXTURE3D_FORMAT::R32F:
            case TEXTURE3D_FORMAT::RG32F:
            case TEXTURE3D_FORMAT::RGB32F:
            case TEXTURE3D_FORMAT::RGBA32F: return GL_FLOAT;

            case TEXTURE3D_FORMAT::R32UI:
            case TEXTURE3D_FORMAT::RG32UI:
            case TEXTURE3D_FORMAT::RGB32UI:
            case TEXTURE3D_FORMAT::RGBA32UI: return GL_UNSIGNED_INT;

            default: return GL_UNSIGNED_BYTE;
        }
    }

    GLenum Texture3D::ResolveFilter(TEXTURE3D_FILTER filter)
    {
        switch (filter)
        {
            case TEXTURE3D_FILTER::NEAREST: return GL_NEAREST;
            case TEXTURE3D_FILTER::LINEAR: return GL_LINEAR;
            case TEXTURE3D_FILTER::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            case TEXTURE3D_FILTER::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
            case TEXTURE3D_FILTER::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
            case TEXTURE3D_FILTER::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
            case TEXTURE3D_FILTER::ANISOTROPIC: return GL_LINEAR_MIPMAP_LINEAR;
            default: return GL_LINEAR;
        }
    }

    GLenum Texture3D::ResolveWrap(TEXTURE3D_WRAP wrap)
    {
        switch (wrap)
        {
            case TEXTURE3D_WRAP::REPEAT: return GL_REPEAT;
            case TEXTURE3D_WRAP::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
            case TEXTURE3D_WRAP::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case TEXTURE3D_WRAP::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            case TEXTURE3D_WRAP::MIRROR_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
            default: return GL_REPEAT;
        }
    }
}