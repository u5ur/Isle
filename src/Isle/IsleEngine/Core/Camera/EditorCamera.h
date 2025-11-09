#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
    class ISLEENGINE_API EditorCamera : public Singleton<EditorCamera>, public SceneComponent
    {
    public:
        Camera* m_Camera;
        glm::vec3 m_Target;
        float m_Distance;

    public:
        virtual void Start() override;
        virtual void Update(float delta_time) override;
        virtual void Destroy() override;
    };
}