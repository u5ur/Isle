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
        Ref<Shader> m_Shader = nullptr;
        Ref<FrameBuffer> m_FrameBuffer = nullptr;
        Ref<PipelineState> m_PipelineState = nullptr;

    public:
        void BlitToScreen();
        virtual void Bind();
        virtual void Unbind();
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        Ref<Shader> GetShader() { return m_Shader; }
        void SetShader(Ref<Shader> shader) { m_Shader = shader; }

        Ref<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }
        void SetFrameBuffer(Ref<FrameBuffer> buffer) { m_FrameBuffer = buffer; }

        Ref<PipelineState> GetPipelineState() { return m_PipelineState; }
        void SetPipelineState(Ref<PipelineState> state) { m_PipelineState = state; }
    };
}
