// Texture3D.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>

namespace Isle
{
    enum class TEXTURE3D_FORMAT
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

    enum class TEXTURE3D_FILTER
    {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR,
        ANISOTROPIC
    };

    enum class TEXTURE3D_WRAP
    {
        REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRRORED_REPEAT,
        MIRROR_CLAMP_TO_EDGE
    };

    class Texture3D : public GfxResource
    {
    public:
        GLuint m_Id = 0;
        int m_Width = 0;
        int m_Height = 0;
        int m_Depth = 0;
        int m_Slot = -1;
        int m_ImageSlot = -1;
        int m_BindlessIndex = -1;

        TEXTURE3D_FORMAT m_Format = TEXTURE3D_FORMAT::RGBA8;
        TEXTURE3D_FILTER m_MinFilter = TEXTURE3D_FILTER::LINEAR;
        TEXTURE3D_FILTER m_MagFilter = TEXTURE3D_FILTER::LINEAR;
        TEXTURE3D_WRAP m_WrapS = TEXTURE3D_WRAP::REPEAT;
        TEXTURE3D_WRAP m_WrapT = TEXTURE3D_WRAP::REPEAT;
        TEXTURE3D_WRAP m_WrapR = TEXTURE3D_WRAP::REPEAT;
        bool m_GenerateMipmaps = false;

    public:
        Texture3D() = default;
        ~Texture3D() override;

        void Create(int width, int height, int depth, TEXTURE3D_FORMAT format,
                   const void* data = nullptr, bool generateMipmaps = false);
        void Destroy();

        void Load() override;
        void Unload() override { Destroy(); }
        void Bind(uint32_t slot = 0) override;
        void Unbind(uint32_t slot = 0) override;

        void BindAsImage(uint32_t slot, GLenum access = GL_READ_WRITE);
        void UnbindAsImage(uint32_t slot);

        void SetMinFilter(TEXTURE3D_FILTER filter);
        void SetMagFilter(TEXTURE3D_FILTER filter);
        void SetWrap(TEXTURE3D_WRAP wrapS, TEXTURE3D_WRAP wrapT, TEXTURE3D_WRAP wrapR);
        void SetBorderColor(const glm::vec4& color);

        void Upload(const void* data, int level = 0);
        void Download(void* outData, int level = 0) const;
        void GenerateMipmaps();
        void Clear(const glm::vec4& clearColor = glm::vec4(0.0f));
        void Resize(int width, int height, int depth);

        void SetDebugLabel(const std::string& name);

        static GLenum ResolveInternalFormat(TEXTURE3D_FORMAT format);
        static GLenum ResolveFormat(TEXTURE3D_FORMAT format);
        static GLenum ResolveDataType(TEXTURE3D_FORMAT format);
        static GLenum ResolveFilter(TEXTURE3D_FILTER filter);
        static GLenum ResolveWrap(TEXTURE3D_WRAP wrap);
    };
}