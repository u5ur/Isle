#include <Core/Common/EditorCommon.h>
#include <Core/AssetManager/AssetManager.h>
#include <Core/Graphics/Texture/Texture.h>
#include <Core/Graphics/Material/Material.h>
#include <Core/Graphics/Mesh/StaticMesh.h>
#include <algorithm>
#include <sstream>

namespace Isle
{
    class Editor::AssetBrowser : public EditorComponent
    {
    private:
        struct AssetEntry
        {
            int id;
            Object* obj;
            std::string name;
            std::string vfsPath;
            std::string type;
        };

        struct VFSFolder
        {
            std::string name;
            std::string fullPath;
            std::vector<VFSFolder*> children;
            std::vector<int> assetIds;
            VFSFolder* parent = nullptr;
        };

        VFSFolder* m_RootFolder = nullptr;
        VFSFolder* m_CurrentFolder = nullptr;
        std::vector<AssetEntry> m_CachedAssets;
        int m_SelectedAssetId = -1;
        float m_ThumbnailSize = 80.0f;
        bool m_ShowGridView = true;

        bool m_IsDraggingAsset = false;
        int m_DraggedAssetId = -1;
        std::string m_DragDropPayloadType = "ASSET_ROOT_OBJECT";

        int m_CurrentAssetId = -1;
        int m_LastAssetCount = 0;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Asset Browser"; }

    private:
        void RebuildVFS();
        void BuildFolderStructure();
        void ClearVFS(VFSFolder* folder);
        VFSFolder* GetOrCreateFolder(const std::string& vfsPath);
        std::vector<std::string> SplitPath(const std::string& path);

        void DrawSplitView();
        void DrawFolderTree(VFSFolder* folder, int depth = 0);
        void DrawAssetGrid();
        void DrawAssetList();

        const char* GetAssetIcon(const std::string& type);
        ImVec4 GetAssetColor(const std::string& type);
        std::vector<AssetEntry*> GetAssetsInCurrentFolder();
        std::vector<AssetEntry*> GetObjectsInAsset(int assetId);

        void HandleAssetDragAndDrop(AssetEntry* entry);
        void HandleRootAssetDragAndDrop(int assetId);
        void ProcessAssetDrop();
    };

    void Editor::AssetBrowser::Start()
    {
        m_RootFolder = new VFSFolder{ "Root", "/", {}, {}, nullptr };
        m_CurrentFolder = m_RootFolder;
        m_CurrentAssetId = -1;
        m_LastAssetCount = 0;
        RebuildVFS();
    }

    void Editor::AssetBrowser::RebuildVFS()
    {
        ClearVFS(m_RootFolder);
        m_RootFolder->children.clear();
        m_RootFolder->assetIds.clear();
        m_CachedAssets.clear();

        AssetManager* assetManager = AssetManager::Instance();
        if (!assetManager)
            return;

        for (auto& [assetId, asset] : assetManager->m_Assets)
        {
            std::string assetName = asset->GetName().empty() ? ("Asset_" + std::to_string(assetId)) : asset->GetName();
            std::string assetPath = "/" + assetName;
            VFSFolder* assetFolder = GetOrCreateFolder(assetPath);

            for (auto& [objId, obj] : asset->m_Objects)
            {
                AssetEntry entry;
                entry.id = objId;
                entry.obj = obj;
                entry.name = obj->GetName().empty() ? ("Object_" + std::to_string(objId)) : obj->GetName();

                if (dynamic_cast<Texture*>(obj))
                {
                    entry.type = "Texture";
                    entry.vfsPath = assetPath + "/Textures/" + entry.name;
                }
                else if (dynamic_cast<Material*>(obj))
                {
                    entry.type = "Material";
                    entry.vfsPath = assetPath + "/Materials/" + entry.name;
                }
                else if (dynamic_cast<StaticMesh*>(obj))
                {
                    entry.type = "Mesh";
                    entry.vfsPath = assetPath + "/Meshes/" + entry.name;
                }
                else if (dynamic_cast<SceneComponent*>(obj))
                {
                    entry.type = "Component";
                    entry.vfsPath = assetPath + "/Components/" + entry.name;
                }
                else
                {
                    entry.type = "Unknown";
                    entry.vfsPath = assetPath + "/Other/" + entry.name;
                }

                m_CachedAssets.push_back(entry);
            }
        }

        BuildFolderStructure();
    }

    void Editor::AssetBrowser::BuildFolderStructure()
    {
        for (auto& entry : m_CachedAssets)
        {
            auto pathParts = SplitPath(entry.vfsPath);
            if (pathParts.empty()) continue;

            std::string currentPath;
            for (size_t i = 0; i < pathParts.size() - 1; i++)
            {
                currentPath += "/" + pathParts[i];
                GetOrCreateFolder(currentPath);
            }

            std::string folderPath = currentPath.empty() ? "/" : currentPath;
            VFSFolder* folder = GetOrCreateFolder(folderPath);
            folder->assetIds.push_back(entry.id);
        }
    }

    Editor::AssetBrowser::VFSFolder* Editor::AssetBrowser::GetOrCreateFolder(const std::string& vfsPath)
    {
        if (vfsPath == "/" || vfsPath.empty())
            return m_RootFolder;

        auto pathParts = SplitPath(vfsPath);
        VFSFolder* current = m_RootFolder;
        std::string builtPath = "";

        for (const auto& part : pathParts)
        {
            builtPath += "/" + part;

            VFSFolder* found = nullptr;
            for (auto* child : current->children)
            {
                if (child->name == part)
                {
                    found = child;
                    break;
                }
            }

            if (!found)
            {
                found = new VFSFolder{ part, builtPath, {}, {}, current };
                current->children.push_back(found);
            }

            current = found;
        }

        return current;
    }

    std::vector<std::string> Editor::AssetBrowser::SplitPath(const std::string& path)
    {
        std::vector<std::string> result;
        std::stringstream ss(path);
        std::string item;

        while (std::getline(ss, item, '/'))
        {
            if (!item.empty())
                result.push_back(item);
        }

        return result;
    }

    void Editor::AssetBrowser::ClearVFS(VFSFolder* folder)
    {
        if (!folder) return;

        for (auto* child : folder->children)
            ClearVFS(child);

        if (folder != m_RootFolder)
            delete folder;
    }

    void Editor::AssetBrowser::Update()
    {
        AssetManager* assetManager = AssetManager::Instance();
        if (!assetManager)
        {
            ImGui::TextDisabled("AssetManager not initialized.");
            return;
        }

        if (assetManager->m_Assets.size() != m_LastAssetCount)
        {
            RebuildVFS();
            m_LastAssetCount = assetManager->m_Assets.size();
        }

        DrawSplitView();
    }


    void Editor::AssetBrowser::DrawSplitView()
    {
        ImGui::BeginChild("FolderTree", ImVec2(250, 0), true);

        if (m_CurrentAssetId != -1)
        {
            if (ImGui::Button("< Back to Assets"))
            {
                m_CurrentAssetId = -1;
            }
            ImGui::Separator();
        }

        if (ImGui::Button("Reload Assets"))
        {
            RebuildVFS();
            m_CurrentAssetId = -1;
        }

        ImGui::Separator();
        DrawFolderTree(m_RootFolder);
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("AssetView", ImVec2(0, 0), true);

        if (m_ShowGridView)
            DrawAssetGrid();
        else
            DrawAssetList();

        ImGui::EndChild();
    }

    void Editor::AssetBrowser::DrawFolderTree(VFSFolder* folder, int depth)
    {
        if (!folder) return;

        bool isRoot = (folder == m_RootFolder);

        if (isRoot)
        {
            ImGui::TextUnformatted("Assets");
            ImGui::Separator();

            AssetManager* assetManager = AssetManager::Instance();
            if (assetManager)
            {
                for (auto& [assetId, asset] : assetManager->m_Assets)
                {
                    if (!asset || !asset->m_RootObject)
                        continue;

                    std::string assetName = asset->GetName().empty() ?
                        ("Asset_" + std::to_string(assetId)) : asset->GetName();

                    bool isCurrent = (m_CurrentAssetId == assetId);
                    if (isCurrent)
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));

                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
                    if (isCurrent)
                        flags |= ImGuiTreeNodeFlags_Selected;

                    bool open = ImGui::TreeNodeEx((void*)(intptr_t)assetId, flags, "%s", assetName.c_str());

                    if (isCurrent)
                        ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        m_CurrentAssetId = assetId;
                    }

                    HandleRootAssetDragAndDrop(assetId);

                    if (open)
                        ImGui::TreePop();
                }
            }
            return;
        }
    }

    std::vector<Editor::AssetBrowser::AssetEntry*> Editor::AssetBrowser::GetAssetsInCurrentFolder()
    {
        std::vector<AssetEntry*> result;

        for (int id : m_CurrentFolder->assetIds)
        {
            auto it = std::find_if(m_CachedAssets.begin(), m_CachedAssets.end(),
                [id](const AssetEntry& e) { return e.id == id; });

            if (it != m_CachedAssets.end())
            {
                result.push_back(&(*it));
            }
        }

        return result;
    }

    std::vector<Editor::AssetBrowser::AssetEntry*> Editor::AssetBrowser::GetObjectsInAsset(int assetId)
    {
        std::vector<AssetEntry*> result;

        AssetManager* assetManager = AssetManager::Instance();
        if (!assetManager)
            return result;

        auto assetIt = assetManager->m_Assets.find(assetId);
        if (assetIt == assetManager->m_Assets.end())
            return result;

        Asset* asset = assetIt->second;

        for (auto& entry : m_CachedAssets)
        {
            for (auto& [objId, obj] : asset->m_Objects)
            {
                if (obj == entry.obj)
                {
                    result.push_back(&entry);
                    break;
                }
            }
        }

        return result;
    }

    void Editor::AssetBrowser::DrawAssetGrid()
    {
        AssetManager* assetManager = AssetManager::Instance();
        if (!assetManager)
        {
            ImGui::TextDisabled("AssetManager not initialized.");
            return;
        }

        if (m_CurrentAssetId != -1)
        {
            auto assetIt = assetManager->m_Assets.find(m_CurrentAssetId);
            if (assetIt != assetManager->m_Assets.end())
            {
                ImGui::Text("Asset: %s", assetIt->second->GetName().c_str());
                ImGui::Separator();
            }
        }

        ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        float cellSize = m_ThumbnailSize + 40.0f;
        int columns = std::max(1, (int)(contentRegion.x / cellSize));

        ImGui::BeginChild("GridScroll", ImVec2(0, 0), false);

        if (m_CurrentAssetId != -1)
        {
            auto objects = GetObjectsInAsset(m_CurrentAssetId);

            if (objects.empty())
            {
                ImGui::TextDisabled("No objects in this asset");
            }
            else
            {
                int currentColumn = 0;

                for (auto* entryPtr : objects)
                {
                    auto& entry = *entryPtr;

                    ImGui::PushID(entry.id);

                    if (currentColumn > 0)
                        ImGui::SameLine();

                    ImGui::BeginGroup();

                    bool selected = (m_SelectedAssetId == entry.id);
                    ImVec4 color = GetAssetColor(entry.type);

                    ImGui::PushStyleColor(ImGuiCol_Button, selected ? ImVec4(0.3f, 0.5f, 0.8f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 0.4f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 0.9f, 0.6f));

                    if (ImGui::Button("##asset_thumb", ImVec2(m_ThumbnailSize, m_ThumbnailSize)))
                    {
                        m_SelectedAssetId = entry.id;
                    }

                    ImGui::PopStyleColor(3);

                    ImVec2 iconPos = ImGui::GetItemRectMin();
                    ImVec2 iconSize = ImGui::GetItemRectSize();
                    const char* icon = GetAssetIcon(entry.type);

                    if (entry.type == "Texture")
                    {
                        Texture* tex = dynamic_cast<Texture*>(entry.obj);
                        if (tex && tex->m_Id != 0)
                        {
                            ImGui::GetWindowDrawList()->AddImage(
                                (ImTextureID)(intptr_t)tex->m_Id,
                                ImVec2(iconPos.x + 4, iconPos.y + 4),
                                ImVec2(iconPos.x + iconSize.x - 4, iconPos.y + iconSize.y - 4),
                                ImVec2(0, 1),
                                ImVec2(1, 0)
                            );
                        }
                        else
                        {
                            ImGui::GetWindowDrawList()->AddRectFilled(
                                ImVec2(iconPos.x + 4, iconPos.y + 4),
                                ImVec2(iconPos.x + iconSize.x - 4, iconPos.y + iconSize.y - 4),
                                IM_COL32(60, 60, 60, 255),
                                4.0f
                            );
                            ImGui::GetWindowDrawList()->AddText(
                                ImVec2(iconPos.x + iconSize.x * 0.35f, iconPos.y + iconSize.y * 0.4f),
                                IM_COL32(255, 255, 255, 128),
                                "T"
                            );
                        }
                    }
                    else
                    {
                        const char* icon = GetAssetIcon(entry.type);
                        ImGui::GetWindowDrawList()->AddText(
                            ImGui::GetFont(),
                            32.0f,
                            ImVec2(iconPos.x + (iconSize.x - ImGui::CalcTextSize(icon).x) * 0.5f,
                                iconPos.y + (iconSize.y - ImGui::CalcTextSize(icon).y) * 0.5f),
                            ImGui::ColorConvertFloat4ToU32(color),
                            icon
                        );
                    }

                    std::string displayName = entry.name;
                    if (displayName.length() > 14)
                        displayName = displayName.substr(0, 11) + "...";

                    ImVec2 nameTextSize = ImGui::CalcTextSize(displayName.c_str());
                    float textPosX = (m_ThumbnailSize - nameTextSize.x) * 0.5f;
                    if (textPosX > 0)
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textPosX);

                    ImGui::TextWrapped("%s", displayName.c_str());

                    ImGui::EndGroup();

                    HandleAssetDragAndDrop(entryPtr);

                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("%s %s", GetAssetIcon(entry.type), entry.name.c_str());
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Type: %s", entry.type.c_str());
                        ImGui::EndTooltip();
                    }

                    ImGui::PopID();

                    currentColumn++;
                    if (currentColumn >= columns)
                        currentColumn = 0;
                }
            }
        }
        else
        {
            bool hasAssets = false;
            int currentColumn = 0;

            for (auto& [assetId, asset] : assetManager->m_Assets)
            {
                if (!asset || !asset->m_RootObject) continue;

                hasAssets = true;

                ImGui::PushID(assetId);

                if (currentColumn > 0)
                    ImGui::SameLine();

                ImGui::BeginGroup();

                bool selected = (m_SelectedAssetId == assetId);
                ImVec4 color = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);

                ImGui::PushStyleColor(ImGuiCol_Button, selected ? ImVec4(0.3f, 0.5f, 0.8f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 0.4f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 0.9f, 0.6f));

                if (ImGui::Button("##asset_thumb", ImVec2(m_ThumbnailSize, m_ThumbnailSize)))
                {
                    m_SelectedAssetId = assetId;
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    m_CurrentAssetId = assetId;
                }

                ImGui::PopStyleColor(3);

                ImVec2 iconPos = ImGui::GetItemRectMin();
                ImVec2 iconSize = ImGui::GetItemRectSize();

                ImGui::GetWindowDrawList()->AddText(
                    ImGui::GetFont(),
                    32.0f,
                    ImVec2(iconPos.x + (iconSize.x - ImGui::CalcTextSize("[A]").x) * 0.5f,
                        iconPos.y + (iconSize.y - ImGui::CalcTextSize("[A]").y) * 0.5f),
                    ImGui::ColorConvertFloat4ToU32(color),
                    "[A]"
                );

                std::string displayName = asset->GetName().empty() ?
                    ("Asset_" + std::to_string(assetId)) : asset->GetName();
                if (displayName.length() > 14)
                    displayName = displayName.substr(0, 11) + "...";

                ImVec2 nameTextSize = ImGui::CalcTextSize(displayName.c_str());
                float textPosX = (m_ThumbnailSize - nameTextSize.x) * 0.5f;
                if (textPosX > 0)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textPosX);

                ImGui::TextWrapped("%s", displayName.c_str());

                ImGui::EndGroup();

                HandleRootAssetDragAndDrop(assetId);

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("[A] %s", displayName.c_str());
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Double-click to view contents");
                    ImGui::EndTooltip();
                }

                ImGui::PopID();

                currentColumn++;
                if (currentColumn >= columns)
                    currentColumn = 0;
            }

            if (!hasAssets)
            {
                ImGui::TextDisabled("No assets loaded");
            }
        }

        ImGui::EndChild();

        ProcessAssetDrop();
    }

    void Editor::AssetBrowser::HandleRootAssetDragAndDrop(int assetId)
    {
        AssetManager* assetManager = AssetManager::Instance();
        if (!assetManager)
            return;

        auto it = assetManager->m_Assets.find(assetId);
        if (it == assetManager->m_Assets.end() || !it->second || !it->second->m_RootObject)
            return;

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload(m_DragDropPayloadType.c_str(), &assetId, sizeof(int));

            ImGui::Text("[A] %s", it->second->GetName().c_str());
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Add to Scene");

            ImGui::EndDragDropSource();
        }
    }

    void Editor::AssetBrowser::HandleAssetDragAndDrop(AssetEntry* entry)
    {
        if (!entry)
            return;

        AssetManager* assetManager = AssetManager::Instance();
        if (!assetManager)
            return;

        Asset* parentAsset = nullptr;
        int assetId = -1;

        for (auto& [aId, asset] : assetManager->m_Assets)
        {
            for (auto& [objId, obj] : asset->m_Objects)
            {
                if (obj == entry->obj)
                {
                    parentAsset = asset;
                    assetId = aId;
                    break;
                }
            }
            if (parentAsset) break;
        }

        if (parentAsset && parentAsset->m_RootObject == entry->obj)
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                ImGui::SetDragDropPayload(m_DragDropPayloadType.c_str(), &assetId, sizeof(int));

                ImGui::Text("[A] %s", parentAsset->GetName().c_str());
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Add to Scene");

                ImGui::EndDragDropSource();
            }
        }
    }

    void Editor::AssetBrowser::ProcessAssetDrop()
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_DragDropPayloadType.c_str()))
            {
                IM_ASSERT(payload->DataSize == sizeof(int));
                int assetId = *(const int*)payload->Data;

                AssetManager* assetManager = AssetManager::Instance();
                if (assetManager)
                {
                    auto it = assetManager->m_Assets.find(assetId);
                    if (it != assetManager->m_Assets.end() && it->second->m_RootObject)
                    {
                        Scene::Instance()->Add(dynamic_cast<SceneComponent*>(it->second->m_RootObject));
                        ISLE_LOG("Added asset '%s' to scene", it->second->GetName().c_str());
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    void Editor::AssetBrowser::DrawAssetList()
    {
        auto assets = GetAssetsInCurrentFolder();

        if (assets.empty())
        {
            ImGui::TextDisabled("No assets in this folder");
            return;
        }

        if (ImGui::BeginTable("AssetTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("VFS Path", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableHeadersRow();

            for (auto* entryPtr : assets)
            {
                auto& entry = *entryPtr;

                ImGui::TableNextRow();
                ImGui::PushID(entry.id);

                ImGui::TableNextColumn();
                bool selected = (m_SelectedAssetId == entry.id);
                if (ImGui::Selectable(entry.name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
                {
                    m_SelectedAssetId = entry.id;
                }

                if (ImGui::IsItemClicked(1))
                {
                    m_SelectedAssetId = entry.id;
                    ImGui::OpenPopup("AssetContextMenu");
                }

                ImGui::TableNextColumn();
                ImVec4 color = GetAssetColor(entry.type);
                ImGui::TextColored(color, "%s %s", GetAssetIcon(entry.type), entry.type.c_str());

                ImGui::TableNextColumn();
                ImGui::TextDisabled("%s", entry.vfsPath.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%d", entry.id);

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        ProcessAssetDrop();
    }

    const char* Editor::AssetBrowser::GetAssetIcon(const std::string& type)
    {
        if (type == "Texture") return "[T]";
        if (type == "Material") return "[M]";
        if (type == "Mesh") return "[3D]";
        if (type == "Component") return "[C]";
        return "[?]";
    }

    ImVec4 Editor::AssetBrowser::GetAssetColor(const std::string& type)
    {
        if (type == "Texture") return ImVec4(0.8f, 0.4f, 0.8f, 1.0f);
        if (type == "Material") return ImVec4(0.4f, 0.8f, 0.4f, 1.0f);
        if (type == "Mesh") return ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
        if (type == "Component") return ImVec4(1.0f, 0.7f, 0.3f, 1.0f);
        return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    }

    void Editor::AssetBrowser::Destroy()
    {
        ClearVFS(m_RootFolder);
        delete m_RootFolder;
        m_CachedAssets.clear();
    }
}