#include "World.h"

namespace Isle
{
	void World::Start()
	{
		AddChild(new DirectionalLight());
	}

	void World::Update(float delta_time)
	{

	}

	void World::Destroy()
	{

	}

	std::vector<Light*> World::GetLights()
	{
		return GetChildren<Light>();
	}
}