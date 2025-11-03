// Texture.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>

namespace Texture
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



    };
}