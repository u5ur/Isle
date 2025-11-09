// Scene.cpp - Safe implementation
#include "Scene.h"
#include <Core/Graphics/Mesh/StaticMesh.h>
#include <Core/Light/Light.h>
#include <Core/Camera/MainCamera.h>
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
        ClearAll();
    }

    void Scene::Add(SceneComponent* component, bool takeOwnership)
    {
        if (!component)
            return;

        if (IsManaged(component))
            return;

        AddChild(component);

        if (takeOwnership)
            m_OwnedComponents.insert(component);
        else
            m_ReferencedComponents.insert(component);

        m_NeedsPipelineUpload = true;
    }

    void Scene::Remove(SceneComponent* component, bool deleteIt)
    {
        if (!component)
            return;

        if (!IsManaged(component))
            return;

        RemoveChild(component);

        bool wasOwned = m_OwnedComponents.erase(component) > 0;
        m_ReferencedComponents.erase(component);

        if (deleteIt && wasOwned)
        {
            DestroyComponent(component, true);
        }

        m_NeedsPipelineUpload = true;
    }

    void Scene::ClearAll()
    {
        for (auto* child : m_Children)
        {
            if (child)
            {
                child->m_Owner = nullptr;
            }
        }

        for (auto* component : m_OwnedComponents)
        {
            if (component)
            {
                SafeDelete(component);
            }
        }

        m_ReferencedComponents.clear();
        m_OwnedComponents.clear();
        m_Children.clear();
        m_NeedsPipelineUpload = true;
    }

    bool Scene::IsManaged(SceneComponent* component) const
    {
        if (!component)
            return false;

        return m_OwnedComponents.count(component) > 0 ||
            m_ReferencedComponents.count(component) > 0;
    }

    bool Scene::IsOwned(SceneComponent* component) const
    {
        if (!component)
            return false;

        return m_OwnedComponents.count(component) > 0;
    }

    void Scene::UploadToPipeline()
    {
        if (!m_NeedsPipelineUpload)
            return;

        auto pipeline = Render::Instance()->GetPipeline();
        if (!pipeline)
            return;

        auto children = GetChildren();
        for (auto& child : children)
        {
            UploadComponentToPipeline(child);
        }

        m_NeedsPipelineUpload = false;
    }

    void Scene::UploadComponentToPipeline(SceneComponent* component)
    {
        if (!component)
            return;

        auto pipeline = Render::Instance()->GetPipeline();
        if (!pipeline)
            return;

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

        try
        {
            component->Start();
        }
        catch (const std::exception& e)
        {
            ISLE_ERROR("Exception starting component '%s': %s\n",
                component->GetName().c_str(), e.what());
            return;
        }

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

        try
        {
            component->Update(delta_time);
        }
        catch (const std::exception& e)
        {
            ISLE_ERROR("Exception updating component '%s': %s\n",
                component->GetName().c_str(), e.what());
            return;
        }

        auto pipeline = Render::Instance()->GetPipeline();
        if (pipeline)
        {
            if (MainCamera* camera = dynamic_cast<MainCamera*>(component))
            {
                pipeline->SetCamera(camera->GetCamera());
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

    void Scene::DestroyComponent(SceneComponent* component, bool deleteIt)
    {
        if (!component)
            return;

        try
        {
            auto children = component->GetChildren();
            for (SceneComponent* child : children)
            {
                bool ownChild = m_OwnedComponents.count(child) > 0;

                if (child)
                {
                    DestroyComponent(child, ownChild);
                }
            }

            component->Destroy();

            if (deleteIt)
            {
                delete component;
            }
        }
        catch (const std::exception& e)
        {
            ISLE_ERROR("Exception destroying component '%s': %s\n",
                component->GetName().c_str(), e.what());
        }
    }

    void Scene::SafeDelete(SceneComponent* component)
    {
        if (!component)
            return;

        try
        {
            auto children = component->GetChildren();

            for (SceneComponent* child : children)
            {
                if (child)
                {
                    if (m_OwnedComponents.count(child) > 0)
                    {
                        SafeDelete(child);
                        m_OwnedComponents.erase(child);
                    }
                    else
                    {
                        child->m_Owner = nullptr;
                    }
                }
            }

            component->m_Children.clear();
            component->m_Owner = nullptr;
            component->Destroy();
            delete component;
        }
        catch (const std::exception& e)
        {
            ISLE_ERROR("Exception in SafeDelete for component: %s\n", e.what());
        }
        catch (...)
        {
            ISLE_ERROR("Unknown exception in SafeDelete\n");
        }
    }
}