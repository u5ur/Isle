// TabBar.h
#pragma once
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    class Editor::TabBar : public EditorComponent
    {
    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "TabBar"; }
    };

    void Editor::TabBar::Start()
    {
    }

    void Editor::TabBar::Update()
    {
        auto* editor = Editor::Instance();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        if (ImGui::BeginTabBar("ViewTabs", ImGuiTabBarFlags_NoTabListScrollingButtons | ImGuiTabBarFlags_FittingPolicyResizeDown))
        {
            if (ImGui::BeginTabItem("Viewport"))
            {
                if (editor->m_CurrentViewMode != ViewMode::VIEWPORT)
                    editor->m_CurrentViewMode = ViewMode::VIEWPORT;

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Code View"))
            {
                if (editor->m_CurrentViewMode != ViewMode::CODEVIEW)
                    editor->m_CurrentViewMode = ViewMode::CODEVIEW;

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::PopStyleVar(2);
    }


    void Editor::TabBar::Destroy()
    {
    }
}