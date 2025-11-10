#pragma once
#include <Core/Common/Common.h>
#include <unordered_map>
#include <queue>

namespace Isle
{
    enum class ComponentState
    {
        PendingStart,
        PendingUpload,
        Active
    };

    struct ComponentInfo
    {
        SceneComponent* component;
        ComponentState state;
        bool owned;
    };

    class ISLEENGINE_API Scene : public Singleton<Scene>, public SceneComponent
    {
    private:
        bool m_IsUploading = false;
        std::unordered_map<SceneComponent*, ComponentInfo> m_Components;
        std::queue<SceneComponent*> m_ProcessQueue;

    public:
        virtual void Start() override;
        virtual void Update(float delta_time) override;
        virtual void Destroy() override;
        void Add(SceneComponent* component, bool takeOwnership = true);
        void Remove(SceneComponent* component, bool deleteIt = false);
        void ClearAll();
        bool IsManaged(SceneComponent* component) const;
        bool IsOwned(SceneComponent* component) const;

    private:
        void StartComponent(SceneComponent* component);
        void UpdateComponent(SceneComponent* component, float delta_time);
        void DestroyComponent(SceneComponent* component, bool deleteIt);
        void UploadComponentToPipeline(SceneComponent* component);
        void SafeDelete(SceneComponent* component);
    };
}