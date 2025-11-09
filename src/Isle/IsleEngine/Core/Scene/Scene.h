#pragma once
#include <Core/Common/Common.h>
#include <unordered_set>

namespace Isle
{
    class ISLEENGINE_API Scene : public Singleton<Scene>, public SceneComponent
    {
    private:
        bool m_NeedsPipelineUpload = true;

        std::unordered_set<SceneComponent*> m_OwnedComponents;
        std::unordered_set<SceneComponent*> m_ReferencedComponents;

    public:
        virtual void Start() override;
        virtual void Update(float delta_time) override;
        virtual void Destroy() override;

        void Add(SceneComponent* component, bool takeOwnership = true);
        void Remove(SceneComponent* component, bool deleteIt = false);

        void ClearAll();

        void UploadToPipeline();

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