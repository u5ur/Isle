// Importer.cpp
#include "Importer.h"
#include <filesystem>

namespace Isle
{
    SceneComponent* Importer::LoadModel(std::string file_path)
    {
        FILE_TYPE fileType = GetFileType(file_path);

        switch (fileType)
        {
        case FILE_TYPE::GLTF:
        case FILE_TYPE::GLB:
        {
            GltfImporter* importer = new GltfImporter();

            if (!importer->LoadFromFile(file_path))
            {
                ISLE_ERROR("Failed to load GLTF file %s\n", file_path.c_str());
                delete importer;
                return nullptr;
            }

            SceneComponent* root = importer->m_RootComponent;

            importer->m_SceneComponents.clear();
            importer->m_RootComponent = nullptr;
            delete importer;

            return root;
        }

        default:
        {
            ISLE_ERROR("Unsupported model type for file %s\n", file_path.c_str());
            return nullptr;
        }
        }
    }

	FILE_TYPE Importer::GetFileType(std::string file_path)
	{
		std::filesystem::path path(file_path);
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".gltf")
			return FILE_TYPE::GLTF;
		else if (ext == ".glb")
			return FILE_TYPE::GLB;
		else if (ext == ".fbx")
			return FILE_TYPE::FBX;

		ISLE_WARN("Unknown file extension '%s', defaulting to GLTF\n", ext.c_str());
		return FILE_TYPE::GLTF;
	}
}