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

    private:
        SceneComponent* m_SelectedComponent = nullptr;

    public:
        Properties* m_Properties;
        Navbar* m_Navbar;
        SceneHierarchy* m_Scene;
        Viewport* m_Viewport;
        AssetBrowser* m_AssetBrowser;
        TransformWidget* m_TransformWidget;

        std::vector<EditorComponent*> m_Components;
        ImVec2 m_ViewportPos;
        ImVec2 m_ViewportSize;

        EditorComponent* m_ResizingComponent = nullptr;
        DOCK_SIDE m_ResizingSide = DOCK_SIDE::NONE;
        ImVec2 m_ResizeStartPos;
        float m_ResizeStartSize;

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

    private:
        void CalculateLayout();
        void RenderComponent(EditorComponent* component);
        void HandleResizing();
        void SelectMesh(Mesh* selectedMesh, bool state);

    };
}