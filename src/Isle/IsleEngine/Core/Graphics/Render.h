#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Pipeline/Pipeline.h>
#include <Core/Graphics/Window/Window.h>

namespace Isle
{
    struct RenderStats
    {
        double RenderTimeCPU = 0.0;
        double RenderTimeGPU = 0.0;

        uint32_t MeshCount = 0;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;
        uint32_t LightCount = 0;
        uint32_t MaterialCount = 0;
        uint32_t TextureCount = 0;

        uint64_t VRAMUsed = 0;
        uint64_t BufferMemoryGPU = 0;
        uint64_t TextureMemoryGPU = 0;

        uint32_t UploadsThisFrame = 0;
        uint32_t DirtyBufferCount = 0;
        uint64_t RenderFrameCount = 0;

        double RenderHz() const { return RenderTimeCPU > 0.0 ? 1000.0 / RenderTimeCPU : 0.0; }

        void ResetPerFrame()
        {
            UploadsThisFrame = 0;
            DirtyBufferCount = 0;
            RenderTimeGPU = 0.0;
        }
    };

    class ISLEENGINE_API Render : public Singleton<Render>, public Component
    {
    private:
        Pipeline* m_Pipeline = nullptr;
        Window* m_Window = nullptr;

        RenderStats m_Stats;
        std::chrono::high_resolution_clock::time_point m_LastFrameTime;

    public:
        void Start(Window* window);
        virtual void Update() override;
        virtual void Destroy() override;

        void BeginFrame(const float* clear_color = nullptr);
        void RenderFrame();
        void EndFrame();
        void Reset();

        Ref<Texture> GetOutputTexture();
        Pipeline* GetPipeline();
        Window* GetWindow();

        const RenderStats& GetStats() const { return m_Stats; }
        RenderStats& GetMutableStats() { return m_Stats; }
    };
}