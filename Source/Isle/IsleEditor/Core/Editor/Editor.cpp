#include "Editor.h"
#include <Core/ModuleManager/ModuleManager.h>
#include <Core/EditorApplication/EditorApplication.h>
#include <Core/Editor/Properties/Properties.h>
#include <Core/Editor/CommandHistory/CommandHistory.h>
#include <Core/Editor/Viewport/Viewport.h>
#include <Core/Editor/TransformWidget/TransformWidget.h>
#include <Core/Editor/Navbar/Navbar.h>
#include <Core/Editor/SceneHierarchy/SceneHierarchy.h>
#include <Core/Editor/AssetBrowser/AssetBrowser.h>
#include <Core/Editor/CodeView/CodeView.h>
#include <Core/Editor/TabBar/TabBar.h>

namespace Isle
{
    void Editor::Start()
    {
        m_Commands = new CommandHistory();
        m_Commands->Start();

        m_Navbar = new Navbar();
        DockConstraints navbarDock;
        navbarDock.m_Side = DOCK_SIDE::TOP;
        navbarDock.m_PreferredSize = 37.5f;
        navbarDock.m_MinSize = 35.0f;
        navbarDock.m_MaxSize = 100.0f;
        navbarDock.m_CanResize = true;
        navbarDock.m_CanMove = true;
        navbarDock.m_Priority = 100;
        navbarDock.m_Showtitle = false;
        m_Navbar->SetDockConstraints(navbarDock);
        m_Navbar->Start();

        m_Properties = new Properties();
        DockConstraints propDock;
        propDock.m_Side = DOCK_SIDE::RIGHT;
        propDock.m_PreferredSize = 300.0f;
        propDock.m_MinSize = 200.0f;
        propDock.m_CanResize = true;
        propDock.m_CanMove = true;
        propDock.m_Priority = 50;
        m_Properties->SetDockConstraints(propDock);
        m_Properties->Start();

        m_Scene = new SceneHierarchy();
        DockConstraints sceneDock;
        sceneDock.m_Side = DOCK_SIDE::RIGHT;
        sceneDock.m_PreferredSize = 300.0f;
        sceneDock.m_MinSize = 150.0f;
        sceneDock.m_CanResize = true;
        sceneDock.m_CanMove = true;
        sceneDock.m_Priority = 51;
        sceneDock.m_StackedAbove = m_Properties;
        sceneDock.m_StackRatio = 0.4f;
        m_Scene->SetDockConstraints(sceneDock);
        m_Scene->Start();

        m_AssetBrowser = new AssetBrowser();
        DockConstraints assetBrowserDock;
        assetBrowserDock.m_Side = DOCK_SIDE::BOTTOM;
        assetBrowserDock.m_PreferredSize = 200.0f;
        assetBrowserDock.m_MinSize = 150.0f;
        assetBrowserDock.m_MaxSize = 400.0f;
        assetBrowserDock.m_CanResize = true;
        assetBrowserDock.m_CanMove = true;
        assetBrowserDock.m_Priority = 40;
        assetBrowserDock.m_Showtitle = true;
        m_AssetBrowser->SetDockConstraints(assetBrowserDock);
        m_AssetBrowser->Start();

        m_TabBar = new TabBar();
        DockConstraints tabBarDock;
        tabBarDock.m_Side = DOCK_SIDE::TOP;
        tabBarDock.m_PreferredSize = 30.0f;
        tabBarDock.m_MinSize = 30.0f;
        tabBarDock.m_MaxSize = 30.0f;
        tabBarDock.m_CanResize = false;
        tabBarDock.m_CanMove = false;
        tabBarDock.m_Showtitle = false;
        tabBarDock.m_Priority = 99;
        m_TabBar->SetDockConstraints(tabBarDock);
        m_TabBar->Start();

        m_Viewport = new Viewport();
        DockConstraints viewportDock;
        viewportDock.m_Side = DOCK_SIDE::FILL;
        viewportDock.m_CanResize = false;
        viewportDock.m_CanMove = false;
        viewportDock.m_Showtitle = false;
        viewportDock.m_Priority = 10;
        m_Viewport->SetDockConstraints(viewportDock);
        m_Viewport->Start();

        m_CodeView = new CodeView();
        DockConstraints codeViewDock;
        codeViewDock.m_Side = DOCK_SIDE::FILL;
        codeViewDock.m_CanResize = false;
        codeViewDock.m_CanMove = false;
        codeViewDock.m_Showtitle = false;
        codeViewDock.m_Priority = 10;
        m_CodeView->SetDockConstraints(codeViewDock);
        m_CodeView->Start();

        m_TransformWidget = new TransformWidget();
        m_TransformWidget->Start();

        m_AssetBrowser->Start();

        m_Components.push_back(m_Navbar);
        m_Components.push_back(m_TabBar);
        m_Components.push_back(m_Properties);
        m_Components.push_back(m_Scene);
        m_Components.push_back(m_AssetBrowser);
        m_Components.push_back(m_Viewport);
        m_Components.push_back(m_CodeView);

        m_CurrentViewMode = ViewMode::VIEWPORT;
    }

    void Editor::Update()
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z) && !io.KeyShift)
        {
            m_Commands->Undo();
        }
        if ((io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z)) ||
            (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)))
        {
            m_Commands->Redo();
        }

        CalculateLayout();
        HandleResizing();

        RenderComponent(m_Navbar);
        RenderComponent(m_TabBar);
        RenderComponent(m_Scene);
        RenderComponent(m_Properties);
        RenderComponent(m_AssetBrowser);

        if (m_CurrentViewMode == ViewMode::VIEWPORT)
        {
            RenderComponent(m_Viewport);
            m_TransformWidget->Update();
        }
        else
        {
            RenderComponent(m_CodeView);
        }
    }

    void Editor::Destroy()
    {
        m_Navbar->Destroy();
        m_TabBar->Destroy();
        m_Properties->Destroy();
        m_Scene->Destroy();
        m_Viewport->Destroy();
        m_CodeView->Destroy();
        m_AssetBrowser->Destroy();
        m_Commands->Destroy();
    }

    void Editor::Add(EditorComponent* component)
    {
        m_Components.push_back(component);
    }

    Editor::Viewport* Editor::GetViewport()
    {
        return m_Viewport;
    }

    Ref<Texture> Editor::GetViewportTexture()
    {
        return m_Viewport->GetViewportTexture();
    }

    void Editor::SetViewportTexture(Ref<Texture> texture)
    {
        m_Viewport->SetViewportTexture(texture);
    }

    ImVec2 Editor::GetViewPortSize()
    {
        return m_Viewport->GetSize();
    }

    SceneComponent* Editor::GetSelectedComponent()
    {
        return m_SelectedComponents.empty() ? nullptr : m_SelectedComponents[0];
    }

    void Editor::SetSelectedComponent(SceneComponent* scene_comp)
    {
        m_SelectedComponents.clear();
        if (scene_comp)
            m_SelectedComponents.push_back(scene_comp);
    }

    void Editor::AddSelectedComponent(SceneComponent* scene_comp)
    {
        if (!scene_comp) return;
        if (std::find(m_SelectedComponents.begin(), m_SelectedComponents.end(), scene_comp) == m_SelectedComponents.end())
            m_SelectedComponents.push_back(scene_comp);
    }

    void Editor::RemoveSelectedComponent(SceneComponent* scene_comp)
    {
        auto it = std::find(m_SelectedComponents.begin(), m_SelectedComponents.end(), scene_comp);
        if (it != m_SelectedComponents.end())
            m_SelectedComponents.erase(it);
    }

    void Editor::ClearSelection()
    {
        m_SelectedComponents.clear();
    }

    bool Editor::IsComponentSelected(SceneComponent* comp)
    {
        return std::find(m_SelectedComponents.begin(), m_SelectedComponents.end(), comp) != m_SelectedComponents.end();
    }

    void Editor::CalculateLayout()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 viewportSize = io.DisplaySize;

        m_ViewportPos = ImVec2(0, 0);
        m_ViewportSize = viewportSize;

        ImVec2 availablePos = ImVec2(0, 0);
        ImVec2 availableSize = viewportSize;

        std::vector<EditorComponent*> sortedComponents;
        for (auto& comp : m_Components)
        {
            if (comp->IsOpen())
                sortedComponents.push_back(comp);
        }
        std::sort(sortedComponents.begin(), sortedComponents.end(),
            [](EditorComponent* a, EditorComponent* b) {
                return a->GetDockConstraints().m_Priority > b->GetDockConstraints().m_Priority;
            });

        for (auto* comp : sortedComponents)
        {
            auto& dock = comp->GetDockConstraints();
            if (dock.m_StackedAbove != nullptr)
                continue;

            float size = dock.m_PreferredSize;
            if (dock.m_MaxSize > 0)
                size = std::min(size, dock.m_MaxSize);
            size = std::max(size, dock.m_MinSize);

            ImVec2 pos, compSize;

            switch (dock.m_Side)
            {
            case DOCK_SIDE::TOP:
                pos = availablePos;
                compSize = ImVec2(availableSize.x, size);
                availablePos.y += size;
                availableSize.y -= size;
                break;

            case DOCK_SIDE::BOTTOM:
                compSize = ImVec2(availableSize.x, size);
                pos = ImVec2(availablePos.x, availablePos.y + availableSize.y - size);
                availableSize.y -= size;
                break;

            case DOCK_SIDE::LEFT:
                pos = availablePos;
                compSize = ImVec2(size, availableSize.y);
                availablePos.x += size;
                availableSize.x -= size;
                break;

            case DOCK_SIDE::RIGHT:
                compSize = ImVec2(size, availableSize.y);
                pos = ImVec2(availablePos.x + availableSize.x - size, availablePos.y);
                availableSize.x -= size;
                break;

            case DOCK_SIDE::FILL:
                pos = availablePos;
                compSize = availableSize;
                break;

            default:
                pos = comp->GetPosition();
                compSize = ImVec2(size, size);
                break;
            }

            comp->SetCalculatedBounds(pos, compSize);
        }

        for (auto* comp : sortedComponents)
        {
            auto& dock = comp->GetDockConstraints();
            if (dock.m_StackedAbove == nullptr)
                continue;

            EditorComponent* parentComp = dock.m_StackedAbove;
            ImVec2 parentPos = parentComp->GetPosition();
            ImVec2 parentSize = parentComp->GetSize();

            float stackHeight = parentSize.y * dock.m_StackRatio;
            stackHeight = std::max(stackHeight, dock.m_MinSize);
            if (dock.m_MaxSize > 0)
                stackHeight = std::min(stackHeight, dock.m_MaxSize);

            float remainingHeight = parentSize.y - stackHeight;
            remainingHeight = std::max(remainingHeight, parentComp->GetDockConstraints().m_MinSize);

            ImVec2 compPos = parentPos;
            ImVec2 compSize = ImVec2(parentSize.x, stackHeight);

            comp->SetCalculatedBounds(compPos, compSize);

            parentComp->SetCalculatedBounds(
                ImVec2(parentPos.x, parentPos.y + stackHeight),
                ImVec2(parentSize.x, remainingHeight)
            );
        }
    }

    void Editor::HandleResizing()
    {
        ImGuiIO& io = ImGui::GetIO();
        const float resizeHandleSize = 4.0f;

        if (m_ResizingComponent && !io.MouseDown[0])
        {
            m_ResizingComponent = nullptr;
            m_ResizingSide = DOCK_SIDE::NONE;
        }

        if (m_ResizingComponent)
        {
            auto& dock = m_ResizingComponent->GetDockConstraints();
            ImVec2 delta = ImVec2(io.MousePos.x - m_ResizeStartPos.x, io.MousePos.y - m_ResizeStartPos.y);
            float newSize = m_ResizeStartSize;

            switch (m_ResizingSide)
            {
            case DOCK_SIDE::TOP:
            case DOCK_SIDE::BOTTOM:
                newSize += (m_ResizingSide == DOCK_SIDE::TOP ? delta.y : -delta.y);
                break;
            case DOCK_SIDE::LEFT:
            case DOCK_SIDE::RIGHT:
                newSize += (m_ResizingSide == DOCK_SIDE::LEFT ? delta.x : -delta.x);
                break;
            }

            newSize = std::max(newSize, dock.m_MinSize);
            if (dock.m_MaxSize > 0)
                newSize = std::min(newSize, dock.m_MaxSize);

            dock.m_PreferredSize = newSize;
            return;
        }

        for (auto& comp : m_Components)
        {
            if (!comp->IsOpen() || !comp->GetDockConstraints().m_CanResize)
                continue;

            ImVec2 pos = comp->GetPosition();
            ImVec2 size = comp->GetSize();
            auto& dock = comp->GetDockConstraints();

            ImRect resizeRect;
            DOCK_SIDE resizeSide = DOCK_SIDE::NONE;

            switch (dock.m_Side)
            {
            case DOCK_SIDE::TOP:
                resizeRect = ImRect(pos.x, pos.y + size.y - resizeHandleSize,
                    pos.x + size.x, pos.y + size.y + resizeHandleSize);
                resizeSide = DOCK_SIDE::TOP;
                break;
            case DOCK_SIDE::BOTTOM:
                resizeRect = ImRect(pos.x, pos.y - resizeHandleSize,
                    pos.x + size.x, pos.y + resizeHandleSize);
                resizeSide = DOCK_SIDE::BOTTOM;
                break;
            case DOCK_SIDE::LEFT:
                resizeRect = ImRect(pos.x + size.x - resizeHandleSize, pos.y,
                    pos.x + size.x + resizeHandleSize, pos.y + size.y);
                resizeSide = DOCK_SIDE::LEFT;
                break;
            case DOCK_SIDE::RIGHT:
                resizeRect = ImRect(pos.x - resizeHandleSize, pos.y,
                    pos.x + resizeHandleSize, pos.y + size.y);
                resizeSide = DOCK_SIDE::RIGHT;
                break;
            }

            if (resizeRect.Contains(io.MousePos))
            {
                if (dock.m_Side == DOCK_SIDE::TOP || dock.m_Side == DOCK_SIDE::BOTTOM)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                else
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                if (io.MouseClicked[0])
                {
                    m_ResizingComponent = comp;
                    m_ResizingSide = resizeSide;
                    m_ResizeStartPos = io.MousePos;
                    m_ResizeStartSize = dock.m_PreferredSize;
                }
            }
        }
    }

    void Editor::RenderComponent(EditorComponent* component)
    {
        if (!component || !component->IsOpen())
            return;

        ImVec2 pos = component->GetPosition();
        ImVec2 size = component->GetSize();

        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);

        const DockConstraints& dock = component->GetDockConstraints();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

        if (!dock.m_Showtitle)
        {
            flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        }
        else
        {
            flags |= ImGuiWindowFlags_NoCollapse;
        }

        if (ImGui::Begin(component->GetWindowName(), nullptr, flags))
        {
            component->Update();
        }
        ImGui::End();
    }
}