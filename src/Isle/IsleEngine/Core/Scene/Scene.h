#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
	class Scene : public Singleton<Scene>, public SceneComponent
	{
	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;
		virtual void Destroy() override;

		void Add(SceneComponent* component);
		void Remove(SceneComponent* component);

	private:
		void StartComponent(SceneComponent* component);
		void UpdateComponent(SceneComponent* component, float delta_time);
		void DestroyComponent(SceneComponent* component);

	};
}