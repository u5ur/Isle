// Engine.h
#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
    class ISLEENGINE_API Engine : public Singleton<Engine>, public Object
    {
    public:
        double m_DeltaTime = 0.0f;
        double m_FPS = 0.0f;

    public:
        void Start();
        void Update();
        void Destroy();
    };
}