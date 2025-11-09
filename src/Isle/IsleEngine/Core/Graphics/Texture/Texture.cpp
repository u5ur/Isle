// Texture.cpp
#include "Texture.h"
#include <stb_image.h>

namespace Isle
{
    Texture::~Texture()
    {
        Destroy();
    }

    void Texture::Create(int width, int height, TEXTURE_FORMAT format,
        const void* data, bool generateMipmaps)
    {
        m_Width = width;
        m_Height = height;
        m_Format = format;
        m_GenerateMipmaps = generateMipmaps;

        if (!m_Id)
            glGenTextures(1, &m_Id);

        glBindTexture(GL_TEXTURE_2D, m_Id);

        GLenum internalFormat = ResolveInternalFormat(format);
        GLenum pixelFormat = ResolveFormat(format);
        GLenum dataType = ResolveDataType(format);

        if (format == TEXTURE_FORMAT::DEPTH16 ||
            format == TEXTURE_FORMAT::DEPTH24 ||
            format == TEXTURE_FORMAT::DEPTH32 ||
            format == TEXTURE_FORMAT::DEPTH32F)
        {
            pixelFormat = GL_DEPTH_COMPONENT;
        }
        else if (format == TEXTURE_FORMAT::DEPTH24_STENCIL8 ||
            format == TEXTURE_FORMAT::DEPTH32F_STENCIL8)
        {
            pixelFormat = GL_DEPTH_STENCIL;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
            pixelFormat, dataType, data);

        SetMinFilter(m_MinFilter);
        SetMagFilter(m_MagFilter);
        SetWrapS(m_WrapS);
        SetWrapT(m_WrapT);

        if (generateMipmaps && data)
            GenerateMipmaps();

        glBindTexture(GL_TEXTURE_2D, 0);

        m_IsLoaded = true;
        m_SizeInBytes = CalculateTextureSize(width, height, format);
    }

    void Texture::CreateFromFile(const std::string& path, bool generateMipmaps)
    {
         int width, height, channels;
         stbi_set_flip_vertically_on_load(true);
         unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
         if (!data)
         {
             ISLE_ERROR("Failed to load texture: %s\n", path.c_str());
             return;
         }
        
         TEXTURE_FORMAT format;
         switch (channels)
         {
             case 1: format = TEXTURE_FORMAT::R8; break;
             case 2: format = TEXTURE_FORMAT::RG8; break;
             case 3: format = TEXTURE_FORMAT::RGB8; break;
             case 4: format = TEXTURE_FORMAT::RGBA8; break;
             default: format = TEXTURE_FORMAT::RGBA8; break;
         }
        
         m_Channels = channels;
         Create(width, height, format, data, generateMipmaps);
        
         stbi_image_free(data);
    }

    void Texture::Destroy()
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

    void Texture::Load()
    {
        if (m_IsLoaded) 
            return;
    }

    void Texture::Bind(uint32_t slot)
    {
        if (!m_Id) return;
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_Id);
        m_Slot = slot;
        m_IsResident = true;
    }

    void Texture::Unbind(uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_IsResident = false;
        m_Slot = -1;
    }

    void Texture::BindAsImage(uint32_t slot, GLenum access)
    {
        if (!m_Id) return;
        GLenum format = ResolveInternalFormat(m_Format);
        glBindImageTexture(slot, m_Id, 0, GL_FALSE, 0, access, format);
        m_ImageSlot = slot;
    }

    void Texture::UnbindAsImage(uint32_t slot)
    {
        glBindImageTexture(slot, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        m_ImageSlot = -1;
    }

    void Texture::SetMinFilter(TEXTURE_FILTER filter)
    {
        m_MinFilter = filter;
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilter(filter));
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetMagFilter(TEXTURE_FILTER filter)
    {
        m_MagFilter = filter;
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilter(filter));
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetWrapS(TEXTURE_WRAP wrap)
    {
        m_WrapS = wrap;
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ResolveWrap(wrap));
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetWrapT(TEXTURE_WRAP wrap)
    {
        m_WrapT = wrap;
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ResolveWrap(wrap));
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetAnisotropicLevel(float level)
    {
        m_AnisotropicLevel = level;
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetBorderColor(const glm::vec4& color)
    {
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Upload(const void* data, int level)
    {
        glBindTexture(GL_TEXTURE_2D, m_Id);
        GLenum format = ResolveFormat(m_Format);
        GLenum type = ResolveDataType(m_Format);
        glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, m_Width, m_Height, format, type, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Download(void* outData, int level) const
    {
        glBindTexture(GL_TEXTURE_2D, m_Id);
        GLenum format = ResolveFormat(m_Format);
        GLenum type = ResolveDataType(m_Format);
        glGetTexImage(GL_TEXTURE_2D, level, format, type, outData);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    uint64_t Texture::GetBindlessHandle()
    {
        if (!m_BindlessHandle)
        {
            m_BindlessHandle = glGetTextureHandleARB(m_Id);
            glMakeTextureHandleResidentARB(m_BindlessHandle);
        }
        return m_BindlessHandle;
    }

    void Texture::GenerateMipmaps()
    {
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Resize(int width, int height)
    {
        m_Width = width;
        m_Height = height;

        glBindTexture(GL_TEXTURE_2D, m_Id);
        GLenum internalFormat = ResolveInternalFormat(m_Format);
        GLenum format = ResolveFormat(m_Format);
        GLenum type = ResolveDataType(m_Format);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_SizeInBytes = width * height * 4;
    }

    void Texture::SetDebugLabel(const std::string& name)
    {
        if (glObjectLabel)
            glObjectLabel(GL_TEXTURE, m_Id, -1, name.c_str());
    }

    GLenum Texture::ResolveInternalFormat(TEXTURE_FORMAT format)
    {
        switch (format)
        {
            case TEXTURE_FORMAT::R8: return GL_R8;
            case TEXTURE_FORMAT::RG8: return GL_RG8;
            case TEXTURE_FORMAT::RGB8: return GL_RGB8;
            case TEXTURE_FORMAT::RGBA8: return GL_RGBA8;
            case TEXTURE_FORMAT::R16F: return GL_R16F;
            case TEXTURE_FORMAT::RG16F: return GL_RG16F;
            case TEXTURE_FORMAT::RGB16F: return GL_RGB16F;
            case TEXTURE_FORMAT::RGBA16F: return GL_RGBA16F;
            case TEXTURE_FORMAT::R32F: return GL_R32F;
            case TEXTURE_FORMAT::RG32F: return GL_RG32F;
            case TEXTURE_FORMAT::RGB32F: return GL_RGB32F;
            case TEXTURE_FORMAT::RGBA32F: return GL_RGBA32F;
            case TEXTURE_FORMAT::R32UI: return GL_R32UI;
            case TEXTURE_FORMAT::RG32UI: return GL_RG32UI;
            case TEXTURE_FORMAT::RGB32UI: return GL_RGB32UI;
            case TEXTURE_FORMAT::RGBA32UI: return GL_RGBA32UI;
            case TEXTURE_FORMAT::DEPTH16: return GL_DEPTH_COMPONENT16;
            case TEXTURE_FORMAT::DEPTH24: return GL_DEPTH_COMPONENT24;
            case TEXTURE_FORMAT::DEPTH32: return GL_DEPTH_COMPONENT32;
            case TEXTURE_FORMAT::DEPTH32F: return GL_DEPTH_COMPONENT32F;
            case TEXTURE_FORMAT::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
            case TEXTURE_FORMAT::DEPTH32F_STENCIL8: return GL_DEPTH32F_STENCIL8;
            case TEXTURE_FORMAT::STENCIL8: return GL_STENCIL_INDEX8;
            default: return GL_RGBA8;
        }
    }

    GLenum Texture::ResolveFormat(TEXTURE_FORMAT format)
    {
        switch (format)
        {
            case TEXTURE_FORMAT::R8:
            case TEXTURE_FORMAT::R16F:
            case TEXTURE_FORMAT::R32F: return GL_RED;

            case TEXTURE_FORMAT::RG8:
            case TEXTURE_FORMAT::RG16F:
            case TEXTURE_FORMAT::RG32F: return GL_RG;

            case TEXTURE_FORMAT::RGB8:
            case TEXTURE_FORMAT::RGB16F:
            case TEXTURE_FORMAT::RGB32F: return GL_RGB;

            case TEXTURE_FORMAT::RGBA8:
            case TEXTURE_FORMAT::RGBA16F:
            case TEXTURE_FORMAT::RGBA32F: return GL_RGBA;

            case TEXTURE_FORMAT::R32UI: return GL_RED_INTEGER;
            case TEXTURE_FORMAT::RG32UI: return GL_RG_INTEGER;
            case TEXTURE_FORMAT::RGB32UI: return GL_RGB_INTEGER;
            case TEXTURE_FORMAT::RGBA32UI: return GL_RGBA_INTEGER;

            case TEXTURE_FORMAT::DEPTH16:
            case TEXTURE_FORMAT::DEPTH24:
            case TEXTURE_FORMAT::DEPTH32:
            case TEXTURE_FORMAT::DEPTH32F: return GL_DEPTH_COMPONENT;

            case TEXTURE_FORMAT::DEPTH24_STENCIL8:
            case TEXTURE_FORMAT::DEPTH32F_STENCIL8: return GL_DEPTH_STENCIL;

            case TEXTURE_FORMAT::STENCIL8: return GL_STENCIL_INDEX;

            default: return GL_RGBA;
        }
    }

    GLenum Texture::ResolveDataType(TEXTURE_FORMAT format)
    {
        switch (format)
        {
            case TEXTURE_FORMAT::R8:
            case TEXTURE_FORMAT::RG8:
            case TEXTURE_FORMAT::RGB8:
            case TEXTURE_FORMAT::RGBA8: return GL_UNSIGNED_BYTE;

            case TEXTURE_FORMAT::R16F:
            case TEXTURE_FORMAT::RG16F:
            case TEXTURE_FORMAT::RGB16F:
            case TEXTURE_FORMAT::RGBA16F: return GL_HALF_FLOAT;

            case TEXTURE_FORMAT::R32F:
            case TEXTURE_FORMAT::RG32F:
            case TEXTURE_FORMAT::RGB32F:
            case TEXTURE_FORMAT::RGBA32F:
            case TEXTURE_FORMAT::DEPTH32F: return GL_FLOAT;

            case TEXTURE_FORMAT::R32UI:
            case TEXTURE_FORMAT::RG32UI:
            case TEXTURE_FORMAT::RGB32UI:
            case TEXTURE_FORMAT::RGBA32UI: return GL_UNSIGNED_INT;

            case TEXTURE_FORMAT::DEPTH16: return GL_UNSIGNED_SHORT;
            case TEXTURE_FORMAT::DEPTH24: return GL_UNSIGNED_INT;
            case TEXTURE_FORMAT::DEPTH32: return GL_UNSIGNED_INT;

            case TEXTURE_FORMAT::DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
            case TEXTURE_FORMAT::DEPTH32F_STENCIL8: return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

            case TEXTURE_FORMAT::STENCIL8: return GL_UNSIGNED_BYTE;

            default: return GL_UNSIGNED_BYTE;
        }
    }

    GLenum Texture::ResolveFilter(TEXTURE_FILTER filter)
    {
        switch (filter)
        {
            case TEXTURE_FILTER::NEAREST: return GL_NEAREST;
            case TEXTURE_FILTER::LINEAR: return GL_LINEAR;
            case TEXTURE_FILTER::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            case TEXTURE_FILTER::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
            case TEXTURE_FILTER::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
            case TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
            case TEXTURE_FILTER::ANISOTROPIC: return GL_LINEAR_MIPMAP_LINEAR;
            default: return GL_LINEAR;
        }
    }

    GLenum Texture::ResolveWrap(TEXTURE_WRAP wrap)
    {
        switch (wrap)
        {
            case TEXTURE_WRAP::REPEAT: return GL_REPEAT;
            case TEXTURE_WRAP::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
            case TEXTURE_WRAP::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case TEXTURE_WRAP::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            case TEXTURE_WRAP::MIRROR_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
            default: return GL_REPEAT;
        }
    }

    int Texture::CalculateTextureSize(int width, int height, TEXTURE_FORMAT format)
    {
        int bytesPerPixel = 4;

        switch (format)
        {
        case TEXTURE_FORMAT::R8: bytesPerPixel = 1; break;
        case TEXTURE_FORMAT::RG8: bytesPerPixel = 2; break;
        case TEXTURE_FORMAT::RGB8: bytesPerPixel = 3; break;
        case TEXTURE_FORMAT::RGBA8: bytesPerPixel = 4; break;
        case TEXTURE_FORMAT::R16F: bytesPerPixel = 2; break;
        case TEXTURE_FORMAT::RG16F: bytesPerPixel = 4; break;
        case TEXTURE_FORMAT::RGB16F: bytesPerPixel = 6; break;
        case TEXTURE_FORMAT::RGBA16F: bytesPerPixel = 8; break;
        case TEXTURE_FORMAT::R32F: bytesPerPixel = 4; break;
        case TEXTURE_FORMAT::RG32F: bytesPerPixel = 8; break;
        case TEXTURE_FORMAT::RGB32F: bytesPerPixel = 12; break;
        case TEXTURE_FORMAT::RGBA32F: bytesPerPixel = 16; break;
        case TEXTURE_FORMAT::DEPTH16: bytesPerPixel = 2; break;
        case TEXTURE_FORMAT::DEPTH24: bytesPerPixel = 3; break;
        case TEXTURE_FORMAT::DEPTH32: bytesPerPixel = 4; break;
        case TEXTURE_FORMAT::DEPTH32F: bytesPerPixel = 4; break;
        case TEXTURE_FORMAT::DEPTH24_STENCIL8: bytesPerPixel = 4; break;
        case TEXTURE_FORMAT::DEPTH32F_STENCIL8: bytesPerPixel = 8; break;
        default: bytesPerPixel = 4; break;
        }

        return width * height * bytesPerPixel;
    }
}