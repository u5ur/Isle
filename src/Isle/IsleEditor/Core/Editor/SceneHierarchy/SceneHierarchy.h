// Scene.h
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    class Editor::SceneHierarchy : public EditorComponent
    {
    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Scene Hierarchy"; }

    private:
        void DrawComponentTree(SceneComponent* component);
    };

    void Editor::SceneHierarchy::Start()
    {
    }

    void Editor::SceneHierarchy::Update()
    {
        ImGui::Begin("Scene Hierarchy");

        Isle::Scene* runtimeScene = Isle::Scene::Instance();
        if (!runtimeScene)
        {
            ImGui::Text("No active scene.");
            ImGui::End();
            return;
        }

        auto children = runtimeScene->GetChildren();
        if (children.empty())
        {
            ImGui::Text("Scene is empty.");
        }
        else
        {
            for (auto& child : children)
                DrawComponentTree(child);
        }

        ImGui::End();
    }

    void Editor::SceneHierarchy::DrawComponentTree(SceneComponent* component)
    {
        if (!component)
            return;

        const std::string& name = component->GetName().empty()
            ? "Unknown"
            : component->GetName();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;
        if (component->GetChildren().empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool open = ImGui::TreeNodeEx((void*)component, flags, "%s", name.c_str());

        if (ImGui::IsItemClicked())
        {
            Editor::Instance()->SetSelectedComponent(component);
        }

        if (open)
        {
            for (auto& child : component->GetChildren())
                DrawComponentTree(child);

            ImGui::TreePop();
        }
    }

    void Editor::SceneHierarchy::Destroy()
    {
    }
}