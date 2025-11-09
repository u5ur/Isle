// FrameBuffer.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>
#include <Core/Graphics/Texture/Texture.h>

namespace Isle
{
    enum class ATTACHMENT_TYPE
    {
        NONE = 0,
        COLOR,
        DEPTH,
        DEPTH_STENCIL,
        NORMAL,
        EMISSIVE,
        POSITION,
        MATERIAL,
        VELOCITY,
        LIGHTING,
        INDIRECT,
        SPECULAR,
        REFLECTION,
        SELECTION,
        RADIANCE,
        SCENE,
        HDR,
        FINAL,
        SHADOW_MAP,
        SHADOW_CUBE
    };

    class FrameBuffer : public GfxResource
    {
    public:
        GLuint m_Id = 0;
        int m_Width;
        int m_Height;

        std::map<ATTACHMENT_TYPE, Ref<Texture>> m_Attachments;
        std::vector<GLenum> m_DrawBuffers;

    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer() override;

        void Create();
        void Destroy();
        void Load() override;
        void Unload() override { Destroy(); }
        void Bind(uint32_t slot = 0) override;
        void Unbind(uint32_t slot = 0) override;

        Ref<Texture> AddAttachment(ATTACHMENT_TYPE type, TEXTURE_FORMAT format = TEXTURE_FORMAT::RGBA16F, bool generateMipmaps = false);
        void AttachTexture(ATTACHMENT_TYPE type, Ref<Texture> texture, int attachmentIndex = 0);
        void AttachDepthTexture(Ref<Texture> texture);
        void AttachDepthStencilTexture(Ref<Texture> texture);
        Ref<Texture> GetAttachment(ATTACHMENT_TYPE type);

        void SetDrawBuffers(const std::vector<ATTACHMENT_TYPE>& targets);
        bool CheckStatus();
        void Clear(const glm::vec4& color = glm::vec4(0.0f), float depth = 1.0f, int stencil = 0);
        void ClearColor(const glm::vec4& color = glm::vec4(0.0f));
        void ClearDepth(float depth = 1.0f);
        void Resize(int width, int height);
        void BlitTo(FrameBuffer* target, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);
        void BlitTo(FrameBuffer* target, const glm::ivec4& srcRect, const glm::ivec4& destRect, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);
        void BlitToTexture(Ref<Texture> targetTexture, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);
        void SetDebugLabel(const std::string& name);
        void SetViewport();

        static GLenum ResolveAttachment(ATTACHMENT_TYPE type, int index = 0);
    };
}