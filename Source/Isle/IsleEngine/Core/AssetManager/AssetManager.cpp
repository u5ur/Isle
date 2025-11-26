#include "AssetManager.h"
#include <Core/Graphics/Material/Material.h>
#include <Core/Graphics/Mesh/StaticMesh.h>
#include <Core/Graphics/Texture/Texture.h>

namespace Isle
{
	Asset* AssetManager::Load(const std::string& path)
	{
		Importer* importer = GetImporter();
		if (!importer)
		{
			ISLE_ERROR("AssetManager: Importer not initialized!\n");
			return nullptr;
		}

		FILE_TYPE fileType = importer->GetFileType(path);
		if (fileType != FILE_TYPE::GLTF && fileType != FILE_TYPE::GLB)
		{
			ISLE_ERROR("AssetManager: Unsupported file type for '%s'\n", path.c_str());
			return nullptr;
		}

		auto* gltfImporter = new GltfImporter();
		if (!gltfImporter->LoadFromFile(path))
		{
			ISLE_ERROR("AssetManager: Failed to load GLTF '%s'\n", path.c_str());
			delete gltfImporter;
			return nullptr;
		}

		int assetId = static_cast<int>(m_Assets.size());
		Asset* asset = new Asset();
		m_Assets[assetId] = asset;

		RegisterAsset(gltfImporter, asset);

		SceneComponent* root = gltfImporter->m_RootComponent;
		if (!root)
		{
			ISLE_WARN("AssetManager: No root component found in '%s'\n", path.c_str());
			delete gltfImporter;
			return nullptr;
		}

		ISLE_LOG("AssetManager: Loaded '%s' (%zu objects)\n", path.c_str(), asset->m_Objects.size());
		delete gltfImporter;
		return asset;
	}

	void AssetManager::RegisterAsset(GltfImporter* importer, Asset* asset)
	{
		asset->m_RootObject = importer->m_RootComponent;
		asset->SetName(importer->m_RootComponent->GetName());

		auto Register = [&](Object* obj)
			{
				if (obj)
				{
					int id = static_cast<int>(asset->m_Objects.size());
					asset->m_Objects[id] = obj;
				}
			};

		for (auto* tex : importer->m_Textures)
			Register(tex);
		for (auto* mat : importer->m_Materials)
			Register(mat);
		for (auto* mesh : importer->m_StaticMeshes)
			Register(mesh);
		for (auto* comp : importer->m_SceneComponents)
			Register(comp);

		if (importer->m_RootComponent)
			Register(importer->m_RootComponent);
	}

	Importer* AssetManager::GetImporter()
	{
		if (!m_Importer)
			m_Importer = new Importer();
		return m_Importer;
	}
}
