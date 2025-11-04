// Pass.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/FrameBuffer/FrameBuffer.h>
#include <Core/Graphics/Shader/Shader.h>
#include <Core/Graphics/PipelineState/PipelineState.h>

namespace Isle
{
    class Pass : public Component
    {
    public:
        Shader* m_Shader = nullptr;
        FrameBuffer* m_FrameBuffer = nullptr;
        PipelineState* m_PipelineState = nullptr;

    public:
        void BlitToScreen();

        virtual void Bind();
        virtual void Unbind();
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        Shader* GetShader();
        void SetShader(Shader* shader);

        FrameBuffer* GetFrameBuffer();
        void SetFrameBuffer(FrameBuffer* buffer);

        PipelineState* GetPipelineState();
        void SetPipelineState(PipelineState* pipeline_state);
    };
}

