// Pass.cpp
#include "Pass.h"

namespace Isle
{
    void Pass::Start()
    {
    }

    void Pass::Update()
    {
    }

    void Pass::Destroy()
    {
    }

    void Pass::Bind()
    {
    }

    void Pass::Unbind()
    {
    }

    void Pass::BlitToScreen()
    {
        if (m_FrameBuffer)
        {
            m_FrameBuffer->BlitTo(nullptr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }
}