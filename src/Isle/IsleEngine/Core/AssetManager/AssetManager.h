#pragma once
#include <Core/Common/Common.h>
#include <Core/Importer/Importer.h>

namespace Isle
{
	class ISLEENGINE_API Asset : public Object
	{
	public:
		Object* m_RootObject;
		std::map<int, Object*> m_Objects;
	};

	class ISLEENGINE_API AssetManager : public Singleton<AssetManager>, public Object
	{
	private:
		Importer* m_Importer = nullptr;

	public:
		std::map<int, Asset*> m_Assets;

	public:
		Asset* Load(const std::string& path);

	private:
		Importer* GetImporter();
		void RegisterAsset(GltfImporter* importer, Asset* asset);
	};
}
