#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Pipeline/Pipeline.h>
#include <Core/Graphics/Window/Window.h>

namespace Isle
{
    class ISLEENGINE_API Render : public Singleton<Render>, public Component
    {
    private:
        Pipeline* m_Pipeline = nullptr;
        Window* m_Window = nullptr;
        bool m_IsEditorMode = false;

    public:
        void Start(Window* window);
        virtual void Update() override;
        virtual void Destroy() override;

        void BeginFrame(const float* clear_color = nullptr);
        void RenderFrame();
        void EndFrame();
        void Reset();

        void SetEditorMode(bool editorMode) { m_IsEditorMode = editorMode; }
        bool IsEditorMode() const { return m_IsEditorMode; }

        Ref<Texture> GetOutputTexture();
        Pipeline* GetPipeline();
        Window* GetWindow();
    };
}