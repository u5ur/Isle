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

        std::string name;
        try
        {
            const std::string& rawName = component->GetName();

            if (rawName.empty())
                name = "Unnamed";
            else
            {
                name.reserve(rawName.size());
                for (unsigned char c : rawName)
                {
                    if (std::isprint(c) || std::isspace(c))
                        name.push_back(static_cast<char>(c));
                    else
                        name.push_back('?');
                }

                if (name.size() > 128)
                    name = name.substr(0, 128) + "...";
            }
        }
        catch (...)
        {
            name = "CorruptedName";
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        const auto& children = component->GetChildren();
        if (children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool open = false;
        try
        {
            open = ImGui::TreeNodeEx((void*)component, flags, "%s", name.c_str());
        }
        catch (...)
        {
            return;
        }

        if (ImGui::IsItemClicked())
        {
            Editor::Instance()->SetSelectedComponent(component);
        }

        if (open)
        {
            for (auto* child : children)
            {
                if (child && child != component)
                    DrawComponentTree(child);
            }

            ImGui::TreePop();
        }
    }


    void Editor::SceneHierarchy::Destroy()
    {
    }
}