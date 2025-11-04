#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Pipeline/Pipeline.h>
#include <Core/Graphics/Window/Window.h>

namespace Isle
{
    class Render : public Singleton<Render>, public Component
    {
    private:
        Pipeline* m_Pipeline = nullptr;
        Window* m_Window = nullptr;

        bool m_ImGui = true;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        void BeginFrame(const float* clear_color = nullptr);
        void RenderFrame();
        void EndFrame();

        void BeginImGuiFrame();
        void RenderImGuiFrame();
        void EndImGuiFrame();

        Pipeline* GetPipeline();
        Window* GetWindow();
    };
}
