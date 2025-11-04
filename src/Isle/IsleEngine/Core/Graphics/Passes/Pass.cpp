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


    Shader* Pass::GetShader()
    {
        return m_Shader;
    }

    void Pass::SetShader(Shader* shader)
    {
        m_Shader = shader;
    }

    FrameBuffer* Pass::GetFrameBuffer()
    {
        return m_FrameBuffer;
    }

    void Pass::SetFrameBuffer(FrameBuffer* buffer)
    {
        m_FrameBuffer = buffer;
    }

    PipelineState* Pass::GetPipelineState()
    {
        return m_PipelineState;
    }

    void Pass::SetPipelineState(PipelineState* pipeline_state)
    {
        m_PipelineState = pipeline_state;
    }
}