// Scene.h
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    class Editor::AssetBrowser : public EditorComponent
    {
    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Asset Browser"; }
    };

    void Editor::AssetBrowser::Start()
    {
    }

    void Editor::AssetBrowser::Update()
    {
        ImGui::Text("Asset Browser");
    }

    void Editor::AssetBrowser::Destroy()
    {
    }
}