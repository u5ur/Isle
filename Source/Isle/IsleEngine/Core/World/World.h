#pragma once
#include <Core/Common/Common.h>
#include <Core/Light/Light.h>
#include <Core/Camera/Camera.h>

namespace Isle
{
	class World : public Singleton<World>, public SceneComponent
	{
	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;
		virtual void Destroy() override;

		std::vector<Light*> GetLights();
	};
}