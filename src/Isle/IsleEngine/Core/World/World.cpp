#include "World.h"

namespace Isle
{
	void World::Start()
	{
		if (GetChildren<DirectionalLight>().empty())
		{
			auto* dirLight = new DirectionalLight();
			AddChild(dirLight);
		}
	}


	void World::Update(float delta_time)
	{

	}

	void World::Destroy()
	{
		auto lights = GetChildren<Light>();
		for (auto* light : lights)
		{
			if (light)
			{
				light->Destroy();
				delete light;
			}
		}

		SceneComponent::Destroy();
	}

	std::vector<Light*> World::GetLights()
	{
		return GetChildren<Light>();
	}
}