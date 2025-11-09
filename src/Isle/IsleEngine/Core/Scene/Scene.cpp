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

        m_NeedsPipelineUpload = true;
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
        for (SceneComponent* child : m_Children)
        {
            DestroyComponent(child);
        }
        m_Children.clear();
    }

    void Scene::Add(SceneComponent* component)
    {
        if (!component)
            return;

        AddChild(component);
        m_NeedsPipelineUpload = true;
    }

    void Scene::Remove(SceneComponent* component)
    {
        RemoveChild(component);
        m_NeedsPipelineUpload = true;
    }

    void Scene::UploadToPipeline()
    {
        auto pipeline = Render::Instance()->GetPipeline();
        if (!pipeline)
        {
            ISLE_ERROR("No pipeline available for scene upload\n");
            return;
        }

        auto children = GetChildren();
        for (auto& child : children)
        {
            UploadComponentToPipeline(child);
        }

        m_NeedsPipelineUpload = false;
    }

    void Scene::UploadComponentToPipeline(SceneComponent* component)
    {
        auto pipeline = Render::Instance()->GetPipeline();
        if (!pipeline) return;

        if (StaticMesh* mesh = dynamic_cast<StaticMesh*>(component))
        {
            pipeline->AddStaticMesh(mesh);
        }
        else if (Light* light = dynamic_cast<Light*>(component))
        {
            pipeline->AddLight(light);
        }
        else if (Camera* camera = dynamic_cast<Camera*>(component))
        {
            pipeline->SetCamera(camera);
        }

        auto children = component->GetChildren();
        for (auto& child : children)
        {
            UploadComponentToPipeline(child);
        }
    }

    void Scene::StartComponent(SceneComponent* component)
    {
        if (!component)
            return;

        component->Start();

        auto children = component->GetChildren();
        for (auto& child : children)
        {
            StartComponent(child);
        }
    }

    void Scene::UpdateComponent(SceneComponent* component, float delta_time)
    {
        if (!component)
            return;

        component->Update(delta_time);

        auto pipeline = Render::Instance()->GetPipeline();
        if (pipeline)
        {
            if (Camera* camera = dynamic_cast<Camera*>(component))
            {
                pipeline->SetCamera(camera);
            }
            else if (StaticMesh* mesh = dynamic_cast<StaticMesh*>(component))
            {
                pipeline->UpdateStaticMesh(mesh);
            }
            else if (Light* light = dynamic_cast<Light*>(component))
            {
                pipeline->UpdateLight(light);
            }
        }

        auto children = component->GetChildren();
        for (auto& child : children)
        {
            UpdateComponent(child, delta_time);
        }
    }

    void Scene::DestroyComponent(SceneComponent* component)
    {
        if (!component) return;

        auto children = component->GetChildren();
        for (SceneComponent* child : children)
        {
            DestroyComponent(child);
        }

        delete component;
    }
}