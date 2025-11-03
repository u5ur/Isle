#pragma once

#ifdef _WIN32
#ifdef ISLEGAME_EXPORTS
#define ISLEGAME_API extern "C" __declspec(dllexport)
#else
#define ISLEGAME_API extern "C" __declspec(dllimport)
#endif
#else
#define ISLEGAME_API extern "C"
#endif

#include <IsleEngine.h>

namespace Isle
{
    class Application : public Singleton<Application>, public Component
    {
        ISLE_OBJECT_CLASS(Application)

    public:
        Window* m_Window;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        static Application* Create();
    };
}

ISLEGAME_API Isle::Application* CreateApplication();
