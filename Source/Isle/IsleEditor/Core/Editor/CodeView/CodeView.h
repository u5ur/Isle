#pragma once
#include <Core/Common/EditorCommon.h>
#include <filesystem>
#include <fstream>
#include <map>
#include "TextEditor.h"

namespace Isle
{
    class Editor::CodeView : public EditorComponent
    {
    public:
        std::string m_GameSourceDir = "src/Isle/IsleGame";

    private:
        TextEditor m_TextEditor;

        struct FileNode {
            std::string name;
            std::string path;
            bool isDirectory;
            std::vector<FileNode> children;
        };

        FileNode m_RootFileNode;
        std::string m_CurrentFile;
        float m_SplitterPos = 250.0f;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "CodeView"; }

    private:
        void BuildFileTree();
        void BuildFileTreeRecursive(FileNode& node, const std::filesystem::path& path);
        void DrawFileTree(FileNode& node);
        void LoadFile(const std::string& filepath);
        void SaveFile(const std::string& filepath);
        TextEditor::LanguageDefinition GetLanguageFromExtension(const std::string& filepath);
    };

    void Editor::CodeView::Start()
    {
        // Initialize TextEditor with C++ syntax
        m_TextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
        m_TextEditor.SetShowWhitespaces(false);
        m_TextEditor.SetTabSize(4);

        BuildFileTree();
    }

    void Editor::CodeView::Update()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        float availableWidth = contentSize.x;
        float rightPaneWidth = availableWidth - m_SplitterPos - 1.0f;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
        ImGui::BeginChild("FileTree", ImVec2(m_SplitterPos, 0), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::PopStyleColor();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

        ImGui::SetCursorPosY(8);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "FILES");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        DrawFileTree(m_RootFileNode);

        ImGui::PopStyleVar(2);
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
        ImGui::InvisibleButton("##splitter", ImVec2(1.0f, contentSize.y));
        ImGui::PopStyleColor(2);

        if (ImGui::IsItemActive()) {
            m_SplitterPos += ImGui::GetIO().MouseDelta.x;
            m_SplitterPos = ImClamp(m_SplitterPos, 150.0f, availableWidth - 300.0f);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 splitterPos = ImGui::GetItemRectMin();
        drawList->AddRectFilled(
            splitterPos,
            ImVec2(splitterPos.x + 1, splitterPos.y + contentSize.y),
            IM_COL32(60, 60, 60, 255)
        );

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        ImGui::BeginChild("CodeEditor", ImVec2(rightPaneWidth, 0), false);
        ImGui::PopStyleColor();

        if (!m_CurrentFile.empty()) {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::BeginChild("EditorHeader", ImVec2(0, 35), true, ImGuiWindowFlags_NoScrollbar);
            ImGui::PopStyleColor();

            ImGui::SetCursorPosY(8);
            ImGui::SetCursorPosX(12);

            std::filesystem::path filePath(m_CurrentFile);
            ImGui::Text("%s", filePath.filename().string().c_str());

            if (m_TextEditor.IsTextChanged()) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "*");
            }

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60);
            if (ImGui::Button("Save", ImVec2(50, 20)) ||
                (ImGui::IsKeyPressed(ImGuiKey_S) && ImGui::GetIO().KeyCtrl)) {
                SaveFile(m_CurrentFile);
            }

            ImGui::EndChild();

            ImVec2 editorSize = ImGui::GetContentRegionAvail();
            m_TextEditor.Render("##TextEditor", editorSize, false);
        }
        else {
            ImGui::SetCursorPos(ImVec2(20, 20));
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Select a file to view");
        }

        ImGui::EndChild();

        ImGui::PopStyleVar(2);
    }

    void Editor::CodeView::Destroy()
    {
    }

    void Editor::CodeView::BuildFileTree()
    {
        m_RootFileNode = FileNode();
        m_RootFileNode.name = "Source";
        m_RootFileNode.path = m_GameSourceDir;
        m_RootFileNode.isDirectory = true;

        if (std::filesystem::exists(m_GameSourceDir)) {
            BuildFileTreeRecursive(m_RootFileNode, m_GameSourceDir);
        }
    }

    void Editor::CodeView::BuildFileTreeRecursive(FileNode& node, const std::filesystem::path& path)
    {
        std::vector<FileNode> directories;
        std::vector<FileNode> files;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            FileNode child;
            child.name = entry.path().filename().string();
            child.path = entry.path().string();
            child.isDirectory = entry.is_directory();

            if (entry.is_directory()) {
                BuildFileTreeRecursive(child, entry.path());
                directories.push_back(child);
            }
            else {
                files.push_back(child);
            }
        }

        // Directories first, then files
        node.children.insert(node.children.end(), directories.begin(), directories.end());
        node.children.insert(node.children.end(), files.begin(), files.end());
    }

    void Editor::CodeView::DrawFileTree(FileNode& node)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (node.isDirectory) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
        else {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (node.path == m_CurrentFile) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
        }

        bool isOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

        if (ImGui::IsItemClicked() && !node.isDirectory) {
            LoadFile(node.path);
        }

        if (isOpen && node.isDirectory) {
            for (auto& child : node.children) {
                DrawFileTree(child);
            }
            ImGui::TreePop();
        }
    }

    void Editor::CodeView::LoadFile(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (file.is_open()) {
            m_CurrentFile = filepath;
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            m_TextEditor.SetLanguageDefinition(GetLanguageFromExtension(filepath));
            m_TextEditor.SetText(content);
        }
    }

    void Editor::CodeView::SaveFile(const std::string& filepath)
    {
        std::ofstream file(filepath);
        if (file.is_open()) {
            std::string content = m_TextEditor.GetText();
            file << content;
            file.close();

            m_TextEditor.SetText(content);
        }
    }

    TextEditor::LanguageDefinition Editor::CodeView::GetLanguageFromExtension(const std::string& filepath)
    {
        std::filesystem::path path(filepath);
        std::string ext = path.extension().string();

        if (ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".h" || ext == ".hpp") {
            return TextEditor::LanguageDefinition::CPlusPlus();
        }
        else if (ext == ".c") {
            return TextEditor::LanguageDefinition::C();
        }
        else if (ext == ".glsl" || ext == ".vert" || ext == ".frag") {
            return TextEditor::LanguageDefinition::GLSL();
        }
        else if (ext == ".hlsl") {
            return TextEditor::LanguageDefinition::HLSL();
        }
        else if (ext == ".lua") {
            return TextEditor::LanguageDefinition::Lua();
        }
        else if (ext == ".sql") {
            return TextEditor::LanguageDefinition::SQL();
        }
        else if (ext == ".as") {
            return TextEditor::LanguageDefinition::AngelScript();
        }

        return TextEditor::LanguageDefinition::CPlusPlus();
    }
}