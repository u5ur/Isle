#pragma once
#include <Core/Common/EditorCommon.h>
#include <IsleEngine.h>
#include <Core/ModuleManager/ModuleManager.h>
#include <Core/EditorApplication/EditorApplication.h>
#include <Core/Builder/Builder.h>

namespace Isle
{
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

        // File Menu
        if (ImGui::Button("File"))
            ImGui::OpenPopup("FileMenu");

        if (ImGui::BeginPopup("FileMenu"))
        {
            if (ImGui::MenuItem("Load Asset/Model"))
            {
                OPENFILENAMEA ofn;
                char szFile[260] = { 0 };

                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = nullptr;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = "All Supported\0*.gltf;*.glb;*.fbx;*.obj\0GLTF\0*.gltf;*.glb\0FBX\0*.fbx\0OBJ\0*.obj\0All Files\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileNameA(&ofn) == TRUE)
                {
                    AssetManager::Instance()->Load(ofn.lpstrFile);
                }
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Game"))
            ImGui::OpenPopup("GameMenu");

        if (ImGui::BeginPopup("GameMenu"))
        {
            auto editorApp = Isle::EditorApplication::Instance();

            if (ImGui::MenuItem("Build", "Ctrl + B"))
            {
                Builder::Instance()->ReloadGame();
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