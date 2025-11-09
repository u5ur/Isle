// Importer.h
#pragma once
#include <Core/Common/Common.h>
#include "Gltf/GltfImporter.h"

namespace Isle
{
	// current supported types are only gltf/glb

	enum FILE_TYPE
	{
		GLTF,
		GLB,
		FBX, // TODO
	};

	class ISLEENGINE_API Importer : public Singleton<Importer>, public Object
	{
	private:
		GltfImporter* m_GltfImporter = nullptr;

	public:
		SceneComponent* LoadModel(std::string file_path);

	private:
		FILE_TYPE GetFileType(std::string file_path);

	};
}
