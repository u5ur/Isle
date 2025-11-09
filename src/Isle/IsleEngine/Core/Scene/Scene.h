#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
    class ISLEENGINE_API Scene : public Singleton<Scene>, public SceneComponent
    {
    private:
        bool m_NeedsPipelineUpload = true;

    public:
        virtual void Start() override;
        virtual void Update(float delta_time) override;
        virtual void Destroy() override;

        void Add(SceneComponent* component);
        void Remove(SceneComponent* component);
        void UploadToPipeline();

    private:
        void StartComponent(SceneComponent* component);
        void UpdateComponent(SceneComponent* component, float delta_time);
        void DestroyComponent(SceneComponent* component);
        void UploadComponentToPipeline(SceneComponent* component);
    };
}
