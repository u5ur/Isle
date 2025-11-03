// Texture.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>

namespace Isle
{
    enum class TEXTURE_FORMAT
    {
        R8,
        RG8,
        RGB8,
        RGBA8,
        R16F,
        RG16F,
        RGB16F,
        RGBA16F,
        R32F,
        RG32F,
        RGB32F,
        RGBA32F,
        R32UI,
        RG32UI,
        RGB32UI,
        RGBA32UI,
        DEPTH16,
        DEPTH24,
        DEPTH32,
        DEPTH32F,
        DEPTH24_STENCIL8,
        DEPTH32F_STENCIL8,
        STENCIL8
    };

    enum class TEXTURE_FILTER
    {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR,
        ANISOTROPIC
    };

    enum class TEXTURE_WRAP
    {
        REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRRORED_REPEAT,
        MIRROR_CLAMP_TO_EDGE
    };

    class Texture : public GfxResource
    {
    public:
        GLuint m_Id = 0;
        int m_Width = 0;
        int m_Height = 0;
        int m_Channels = 0;
        int m_Slot = -1;
        int m_ImageSlot = -1;
        int m_BindlessIndex = -1;

        TEXTURE_FORMAT m_Format = TEXTURE_FORMAT::RGBA8;
        TEXTURE_FILTER m_MinFilter = TEXTURE_FILTER::LINEAR;
        TEXTURE_FILTER m_MagFilter = TEXTURE_FILTER::LINEAR;
        TEXTURE_WRAP m_WrapS = TEXTURE_WRAP::REPEAT;
        TEXTURE_WRAP m_WrapT = TEXTURE_WRAP::REPEAT;

        bool m_GenerateMipmaps = false;
        float m_AnisotropicLevel = 1.0f;

    public:
        Texture() = default;
        ~Texture() override;

        void Create(int width, int height, TEXTURE_FORMAT format,
                   const void* data = nullptr, bool generateMipmaps = false);
        void CreateFromFile(const std::string& path, bool generateMipmaps = true);
        void Destroy();

        void Load() override;
        void Unload() override { Destroy(); }
        void Bind(uint32_t slot = 0) override;
        void Unbind(uint32_t slot = 0) override;


        void BindAsImage(uint32_t slot, GLenum access = GL_READ_WRITE);
        void UnbindAsImage(uint32_t slot);

        void SetMinFilter(TEXTURE_FILTER filter);
        void SetMagFilter(TEXTURE_FILTER filter);
        void SetWrapS(TEXTURE_WRAP wrap);
        void SetWrapT(TEXTURE_WRAP wrap);
        void SetAnisotropicLevel(float level);
        void SetBorderColor(const glm::vec4& color);

        void Upload(const void* data, int level = 0);
        void Download(void* outData, int level = 0) const;
        void GenerateMipmaps();
        void Resize(int width, int height);

        void SetDebugLabel(const std::string& name);

        static GLenum ResolveInternalFormat(TEXTURE_FORMAT format);
        static GLenum ResolveFormat(TEXTURE_FORMAT format);
        static GLenum ResolveDataType(TEXTURE_FORMAT format);
        static GLenum ResolveFilter(TEXTURE_FILTER filter);
        static GLenum ResolveWrap(TEXTURE_WRAP wrap);
    };
}