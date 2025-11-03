// FrameBuffer.h
#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
    class Texture;

    enum class RENDER_TARGET_TYPE
    {
        NONE = 0,
        COLOR,
        DEPTH,
        DEPTH_STENCIL,
        NORMAL,
        EMISSIVE,
        MATERIAL,
        VELOCITY,
        LIGHTING,
        INDIRECT,
        SPECULAR,
        REFLECTION,
        RADIANCE,
        SCENE,
        HDR,
        FINAL
    };

	class FrameBuffer : public Component
	{
	public:
        int m_Width;
        int m_Height;

        std::map<RENDER_TARGET_TYPE, Texture*> m_Attachments;
        std::vector<GLenum> m_DrawBuffers;

    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer();

        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
	};
}