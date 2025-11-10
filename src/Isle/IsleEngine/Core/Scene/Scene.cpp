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
        for (auto& child : GetChildren())
        {
            if (!child || !child->IsValid())
                continue;

            m_Components[child] = { child, ComponentState::PendingStart, false };
            m_ProcessQueue.push(child);
        }
        m_IsUploading = true;
    }

    void Scene::Update(float delta_time)
    {
        // Process everything in batches - no artificial frame limits
        while (!m_ProcessQueue.empty())
        {
            // Collect all components to start
            std::vector<SceneComponent*> toStart;
            std::vector<SceneComponent*> toUpload;

            size_t queueSize = m_ProcessQueue.size();
            for (size_t i = 0; i < queueSize; ++i)
            {
                SceneComponent* comp = m_ProcessQueue.front();
                m_ProcessQueue.pop();

                if (!comp || !comp->IsValid())
                    continue;

                auto it = m_Components.find(comp);
                if (it == m_Components.end())
                    continue;

                ComponentInfo& info = it->second;

                if (info.state == ComponentState::PendingStart)
                {
                    toStart.push_back(comp);
                }
                else if (info.state == ComponentState::PendingUpload)
                {
                    toUpload.push_back(comp);
                }
            }

            // Batch start all components
            for (auto* comp : toStart)
            {
                StartComponent(comp);
                auto it = m_Components.find(comp);
                if (it != m_Components.end())
                {
                    it->second.state = ComponentState::PendingUpload;
                    m_ProcessQueue.push(comp);
                }
            }

            // Batch upload all components to pipeline
            if (!toUpload.empty())
            {
                auto pipeline = Render::Instance()->GetPipeline();
                if (pipeline)
                {
                    // Group by type for efficient pipeline operations
                    std::vector<StaticMesh*> meshes;
                    std::vector<Light*> lights;
                    Camera* camera = nullptr;

                    for (auto* comp : toUpload)
                    {
                        if (auto* mesh = dynamic_cast<StaticMesh*>(comp))
                            meshes.push_back(mesh);
                        else if (auto* light = dynamic_cast<Light*>(comp))
                            lights.push_back(light);
                        else if (auto* cam = dynamic_cast<Camera*>(comp))
                            camera = cam;

                        // Handle children
                        for (auto& child : comp->GetChildren())
                        {
                            if (!child || !child->IsValid())
                                continue;

                            if (m_Components.count(child) == 0)
                            {
                                m_Components[child] = { child, ComponentState::PendingStart, false };
                                m_ProcessQueue.push(child);
                            }
                        }
                    }

                    // Batch add to pipeline
                    for (auto* mesh : meshes)
                        pipeline->AddStaticMesh(mesh);
                    for (auto* light : lights)
                        pipeline->AddLight(light);
                    if (camera)
                        pipeline->SetCamera(camera);
                }

                // Mark all as active
                for (auto* comp : toUpload)
                {
                    auto it = m_Components.find(comp);
                    if (it != m_Components.end())
                        it->second.state = ComponentState::Active;
                }
            }

            // If no new work was queued, we're done
            if (toStart.empty() && toUpload.empty())
                break;
        }

        m_IsUploading = !m_ProcessQueue.empty();

        // Update all active components
        for (auto* child : GetChildren())
        {
            if (!child || !child->IsValid())
                continue;

            auto it = m_Components.find(child);
            if (it != m_Components.end() && it->second.state == ComponentState::Active)
                UpdateComponent(child, delta_time);
        }
    }

    void Scene::Destroy()
    {
        ClearAll();
    }

    void Scene::Add(SceneComponent* component, bool takeOwnership)
    {
        if (!component || !component->IsValid())
            return;

        if (m_Components.count(component))
            return;

        AddChild(component);
        m_Components[component] = { component, ComponentState::PendingStart, takeOwnership };
        m_ProcessQueue.push(component);
        m_IsUploading = true;
    }

    void Scene::Remove(SceneComponent* component, bool deleteIt)
    {
        if (!component)
            return;

        auto it = m_Components.find(component);
        if (it == m_Components.end())
            return;

        RemoveChild(component);

        bool wasOwned = it->second.owned;
        m_Components.erase(it);

        if (deleteIt && wasOwned)
            DestroyComponent(component, true);
    }

    void Scene::ClearAll()
    {
        for (auto* child : m_Children)
        {
            if (child && child->IsValid())
                child->m_Owner = nullptr;
        }

        std::vector<SceneComponent*> toDelete;
        for (auto& [comp, info] : m_Components)
        {
            if (comp && info.owned)
                toDelete.push_back(comp);
        }

        for (auto* comp : toDelete)
        {
            SafeDelete(comp);
        }

        m_Components.clear();
        while (!m_ProcessQueue.empty())
            m_ProcessQueue.pop();
        m_Children.clear();
        m_IsUploading = false;
    }

    bool Scene::IsManaged(SceneComponent* component) const
    {
        return component && component->IsValid() && m_Components.count(component) > 0;
    }

    bool Scene::IsOwned(SceneComponent* component) const
    {
        if (!component || !component->IsValid())
            return false;

        auto it = m_Components.find(component);
        return it != m_Components.end() && it->second.owned;
    }

    void Scene::UploadComponentToPipeline(SceneComponent* component)
    {
        if (!component || !component->IsValid())
            return;

        auto pipeline = Render::Instance()->GetPipeline();
        if (!pipeline)
            return;

        if (auto* mesh = dynamic_cast<StaticMesh*>(component))
            pipeline->AddStaticMesh(mesh);
        else if (auto* light = dynamic_cast<Light*>(component))
            pipeline->AddLight(light);
        else if (auto* camera = dynamic_cast<Camera*>(component))
            pipeline->SetCamera(camera);

        for (auto& child : component->GetChildren())
        {
            if (!child || !child->IsValid())
                continue;

            if (m_Components.count(child) == 0)
            {
                m_Components[child] = { child, ComponentState::PendingStart, false };
                m_ProcessQueue.push(child);
            }
        }
    }

    void Scene::StartComponent(SceneComponent* component)
    {
        if (!component || !component->IsValid())
            return;

        try
        {
            component->Start();
        }
        catch (const std::exception& e)
        {
            ISLE_ERROR("Exception starting component '%s': %s\n",
                component->GetName().c_str(), e.what());
        }
        catch (...)
        {
            ISLE_ERROR("Unknown exception starting component '%s'\n",
                component->GetName().c_str());
        }
    }

    void Scene::UpdateComponent(SceneComponent* component, float delta_time)
    {
        if (!component || !component->IsValid())
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
        catch (...)
        {
            ISLE_ERROR("Unknown exception updating component '%s'\n",
                component->GetName().c_str());
            return;
        }

        auto pipeline = Render::Instance()->GetPipeline();
        if (pipeline)
        {
            if (auto* mainCam = dynamic_cast<MainCamera*>(component))
                pipeline->SetCamera(mainCam->GetCamera());
            else if (auto* mesh = dynamic_cast<StaticMesh*>(component))
                pipeline->UpdateStaticMesh(mesh);
            else if (auto* light = dynamic_cast<Light*>(component))
                pipeline->UpdateLight(light);
        }

        for (auto& child : component->GetChildren())
        {
            if (!child || !child->IsValid())
                continue;

            auto it = m_Components.find(child);
            if (it != m_Components.end() && it->second.state == ComponentState::Active)
                UpdateComponent(child, delta_time);
        }
    }

    void Scene::DestroyComponent(SceneComponent* component, bool deleteIt)
    {
        if (!component)
            return;

        try
        {
            std::vector<SceneComponent*> children = component->GetChildren();

            for (auto* child : children)
            {
                if (!child)
                    continue;

                auto it = m_Components.find(child);
                bool ownChild = it != m_Components.end() && it->second.owned;

                if (it != m_Components.end())
                    m_Components.erase(it);

                DestroyComponent(child, ownChild);
            }

            if (component->IsValid())
                component->Destroy();

            if (deleteIt)
                delete component;
        }
        catch (const std::exception& e)
        {
            ISLE_ERROR("Exception destroying component '%s': %s\n",
                component->GetName().c_str(), e.what());
        }
        catch (...)
        {
            ISLE_ERROR("Unknown exception destroying component '%s'\n",
                component->GetName().c_str());
        }
    }

    void Scene::SafeDelete(SceneComponent* component)
    {
        if (!component)
            return;

        try
        {
            std::vector<SceneComponent*> children = component->GetChildren();

            for (auto* child : children)
            {
                if (!child)
                    continue;

                auto it = m_Components.find(child);
                if (it != m_Components.end())
                {
                    if (it->second.owned)
                    {
                        m_Components.erase(it);
                        SafeDelete(child);
                    }
                    else
                    {
                        if (child->IsValid())
                            child->m_Owner = nullptr;
                        m_Components.erase(it);
                    }
                }
                else if (child->IsValid())
                {
                    child->m_Owner = nullptr;
                }
            }

            component->m_Children.clear();
            component->m_Owner = nullptr;

            if (component->IsValid())
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