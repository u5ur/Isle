#include "Scene.h"
#include <Core/Graphics/Mesh/StaticMesh.h>
#include <Core/Light/Light.h>
#include <Core/Camera/Camera.h>
#include <Core/Graphics/Render.h>
#include <Core/Graphics/Pipeline/Pipeline.h>

namespace Isle
{
	void Scene::Start()
	{
		auto children = GetChildren();

		for (auto& child : children)
		{
			StartComponent(child);
		}
	}

	void Scene::Update(float delta_time)
	{
		auto children = GetChildren();

		for (auto& child : children)
		{
			UpdateComponent(child, delta_time);
		}
	}

	void Scene::Destroy()
	{

	}

	void Scene::Add(SceneComponent* component)
	{
		AddChild(component);
	}

	void Scene::Remove(SceneComponent* component)
	{
		RemoveChild(component);
	}

	void Scene::StartComponent(SceneComponent* component)
	{
		component->Start();

		if (StaticMesh* mesh = dynamic_cast<StaticMesh*>(component))
		{
			Render::Instance()->GetPipeline()->AddStaticMesh(mesh);
		}
		else if (Light* light = dynamic_cast<Light*>(component))
		{
			Render::Instance()->GetPipeline()->AddLight(light);
		}
		else if (Camera* camera = dynamic_cast<Camera*>(component))
		{
			Render::Instance()->GetPipeline()->SetCamera(camera);
		}

		auto children = component->GetChildren();
		for (auto& child : children)
		{
			StartComponent(child);
		}
	}

	void Scene::UpdateComponent(SceneComponent* component, float delta_time)
	{
		component->Update(delta_time);

		if (Camera* camera = dynamic_cast<Camera*>(component))
		{
			Render::Instance()->GetPipeline()->SetCamera(camera);
		}

		auto children = component->GetChildren();
		for (auto& child : children)
		{
			UpdateComponent(child, delta_time);
		}
	}

	void Scene::DestroyComponent(SceneComponent* component)
	{

	}
}