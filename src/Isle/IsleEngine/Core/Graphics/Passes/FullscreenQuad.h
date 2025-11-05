#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxBuffer/GfxBuffer.h>

namespace Isle
{
    class FullscreenQuad
    {
    private:
        Ref<GfxBuffer> m_VertexBuffer = nullptr;
        Ref<GfxBuffer> m_IndexBuffer = nullptr;

    public:
        FullscreenQuad();
        ~FullscreenQuad();

        void Create();
        void Destroy();
        void Draw();
    };
}
