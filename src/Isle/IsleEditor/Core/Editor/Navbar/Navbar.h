#pragma once
#include <Core/Common/EditorCommon.h>
#include <IsleEngine.h>
#include <Core/ModuleManager/ModuleManager.h>
#include <Core/EditorApplication/EditorApplication.h>
#include <windows.h>
#include <string>

namespace Isle
{
    class DLLReloader
    {
    public:
        static bool ReloadGameDLL()
        {
            auto editorApp = EditorApplication::Instance();

#ifdef _DEBUG
            const char* dllName = "IsleGame_Debug.dll";
#else
            const char* dllName = "IsleGame.dll";
#endif
            std::string buildDll = std::string("build/") + dllName;

            ISLE_LOG("[1/6] Shutting down game...\n");
            editorApp->ShutdownGame();

            ISLE_LOG("[2/6] Unloading DLL...\n");
            editorApp->UnloadGameDLL();
            Sleep(1500);

            ISLE_LOG("[3/6] Deleting old DLL...\n");
            DeleteFileA(buildDll.c_str());

            ISLE_LOG("[4/6] Building project...\n");
            if (system("cmake --build --preset build-debug") != 0)
            {
                ISLE_ERROR("Build failed!\n");
                ISLE_LOG("Attempting to recover...\n");
                if (editorApp->LoadGameDLL())
                {
                    editorApp->InitializeGame();
                }
                return false;
            }

            ISLE_LOG("Build successful!\n");

            ISLE_LOG("[5/6] Loading new DLL...\n");
            if (!editorApp->LoadGameDLL())
            {
                ISLE_ERROR("Failed to load new DLL!\n");
                return false;
            }

            ISLE_LOG("[6/6] Initializing game...\n");
            if (!editorApp->InitializeGame())
            {
                ISLE_ERROR("Failed to initialize game!\n");
                return false;
            }

            return true;
        }
    };

    class Editor::Navbar : public EditorComponent
    {
    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Navbar"; }
    };

    void Editor::Navbar::Start()
    {
    }

    void Editor::Navbar::Update()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

        auto* editor = Editor::Instance();
        auto* transformWidget = editor->m_TransformWidget;
        if (!transformWidget)
        {
            ImGui::PopStyleVar(2);
            return;
        }

        if (ImGui::Button("Game"))
            ImGui::OpenPopup("GameMenu");

        if (ImGui::BeginPopup("GameMenu"))
        {
            auto editorApp = Isle::EditorApplication::Instance();

            if (ImGui::MenuItem("Reload DLL", "Ctrl+R"))
            {
                DLLReloader::ReloadGameDLL();
            }

            ImGui::Separator();

            if (editorApp->IsPaused())
            {
                if (ImGui::MenuItem("Play", "Space"))
                {
                    editorApp->SetPaused(false);
                    Input::Instance()->SetCursorMode(true);
                }
            }
            else
            {
                if (ImGui::MenuItem("Pause", "Space"))
                {
                    editorApp->SetPaused(true);
                    Input::Instance()->SetCursorMode(false);
                }
            }

            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));

        ImVec2 navPos = m_CalculatedPosition;
        ImVec2 navSize = m_CalculatedSize;

        float translateWidth = ImGui::CalcTextSize("Translate").x + ImGui::GetStyle().FramePadding.x * 4.0f;
        float rotateWidth = ImGui::CalcTextSize("Rotate").x + ImGui::GetStyle().FramePadding.x * 4.0f;
        float scaleWidth = ImGui::CalcTextSize("Scale").x + ImGui::GetStyle().FramePadding.x * 4.0f;
        float totalWidth = translateWidth + rotateWidth + scaleWidth + ImGui::GetStyle().ItemSpacing.x * 4.0f;

        float startX = navPos.x + navSize.x - totalWidth - ImGui::GetStyle().WindowPadding.x;
        ImGui::SetCursorScreenPos(ImVec2(startX, navPos.y + (navSize.y - ImGui::GetFrameHeight()) * 0.5f));

        int currentOp = static_cast<int>(transformWidget->m_GizmoOperation);

        if (ImGui::RadioButton("Translate", currentOp == ImGuizmo::TRANSLATE))
            transformWidget->m_GizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();

        if (ImGui::RadioButton("Rotate", currentOp == ImGuizmo::ROTATE))
            transformWidget->m_GizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();

        if (ImGui::RadioButton("Scale", currentOp == ImGuizmo::SCALE))
            transformWidget->m_GizmoOperation = ImGuizmo::SCALE;

        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);
    }

    void Editor::Navbar::Destroy()
    {
    }
}