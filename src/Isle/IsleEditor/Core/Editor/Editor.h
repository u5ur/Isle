// Editor.h
#pragma once
#include <IsleEngine.h>
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    class Editor : public Singleton<Editor>, public Object
    {
    public:
        class Properties;
        class Navbar;
        class SceneHierarchy;
        class Viewport;
        class AssetBrowser;
        class TransformWidget;
        class CommandHistory;

    private:
        std::vector<SceneComponent*> m_SelectedComponents;

    public:
        Properties* m_Properties;
        Navbar* m_Navbar;
        SceneHierarchy* m_Scene;
        Viewport* m_Viewport;
        AssetBrowser* m_AssetBrowser;
        TransformWidget* m_TransformWidget;
        CommandHistory* m_Commands;

        std::vector<EditorComponent*> m_Components;
        ImVec2 m_ViewportPos;
        ImVec2 m_ViewportSize;

        EditorComponent* m_ResizingComponent = nullptr;
        DOCK_SIDE m_ResizingSide = DOCK_SIDE::NONE;
        ImVec2 m_ResizeStartPos;
        float m_ResizeStartSize;

        bool m_IsSelecting = false;
        ImVec2 m_SelectionStart;
        ImVec2 m_SelectionEnd;
        bool m_MultiSelectMode = false;

    public:
        void Start();
        void Update();
        void Destroy();
        void Add(EditorComponent* component);

        Viewport* GetViewport();
        ImVec2 GetViewPortSize();
        Ref<Texture> GetViewportTexture();
        void SetViewportTexture(Ref<Texture> texture);

        SceneComponent* GetSelectedComponent();
        void SetSelectedComponent(SceneComponent* scene_comp);

        std::vector<SceneComponent*>& GetSelectedComponents() { return m_SelectedComponents; }
        void AddSelectedComponent(SceneComponent* scene_comp);
        void RemoveSelectedComponent(SceneComponent* scene_comp);
        bool IsComponentSelected(SceneComponent* comp);
        void ClearSelection();

    private:
        void CalculateLayout();
        void RenderComponent(EditorComponent* component);
        void HandleResizing();
        void SelectMesh(Mesh* selectedMesh, bool state);
    };
}