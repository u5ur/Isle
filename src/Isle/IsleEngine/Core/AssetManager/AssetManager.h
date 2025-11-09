// AssetManager.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Importer/Importer.h>

namespace Isle
{
	class ISLEENGINE_API AssetManager : public Singleton<AssetManager>, public Object
	{
	private:
		Importer* m_Importer;

	public:
		std::map<int, Material*> m_Materials;
		std::map<int, Texture*> m_Textures;
		std::map<int, StaticMesh*> m_Meshes;

	private:
		Importer* GetImporter();

	};
}
