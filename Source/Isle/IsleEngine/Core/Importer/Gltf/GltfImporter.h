#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Mesh/Mesh.h>
#include <Core/Graphics/Mesh/StaticMesh.h>
#include <Core/Graphics/Texture/Texture.h>

namespace Isle
{
    class GltfImporter
    {
    public:
        SceneComponent* m_RootComponent = nullptr;
        std::vector<StaticMesh*> m_StaticMeshes;
        std::vector<Texture*> m_Textures;
        std::vector<Material*> m_Materials;
        std::vector<SceneComponent*> m_SceneComponents;

    private:
        std::string m_BasePath;
        std::map<int, std::vector<int>> m_MeshToPrimitives;
        std::map<int, SceneComponent*> m_NodeMap;

    public:
        GltfImporter();
        ~GltfImporter();
        bool LoadFromFile(const std::string& file_path);

    private:
        StaticMesh* GetStaticMesh(int mesh_index);
        Material* GetMaterial(int material_index);
        Texture* GetTexture(int texture_index);

        void LoadTextures();
        void LoadMaterials();
        void LoadStaticMeshes();

        void ProcessNode(int node_index, SceneComponent* parent);
        void ExtractBasePath(const std::string& file_path);
    };
}