#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
    class Application;

    template<>
    class ISLEENGINE_API Singleton<Application>
    {
    private:
        static Application* s_Instance;
        static bool s_IsValid;

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;

    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        static Application* Instance();
        static void SetInstance(Application* instance);
        static void ResetInstance();
        static bool IsValid();
    };

    class ISLEENGINE_API Application : public Singleton<Application>, public Object
    {
    public:
        float m_DeltaTime = 0.0f;

        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Destroy() = 0;

        virtual ~Application() = default;
    };
}