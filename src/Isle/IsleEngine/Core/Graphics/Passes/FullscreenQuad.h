#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxBuffer/GfxBuffer.h>

namespace Isle
{
    class FullscreenQuad
    {
    private:
        GfxBuffer* m_VertexBuffer = nullptr;
        GfxBuffer* m_IndexBuffer = nullptr;

    public:
        FullscreenQuad();
        ~FullscreenQuad();

        void Create();
        void Destroy();
        void Draw();
    };
}
