// IsleGame.h
#pragma once
#ifdef _WIN32
#ifdef ISLEGAME_EXPORTS
#define ISLEGAME_API __declspec(dllexport)
#else
#define ISLEGAME_API __declspec(dllimport)
#endif
#else
#define ISLEGAME_API
#endif

#include <IsleEngine.h>

namespace Isle
{
    class ISLEGAME_API Application : public Component
    {
        ISLE_OBJECT_CLASS(Application)
    public:
        float m_DeltaTime = 0.0f;
        bool m_IsEditorMode = false;
        bool m_Paused = false;

        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
    };
}

extern "C" {
    ISLEGAME_API void* CreateApplication();
    ISLEGAME_API void DestroyApplication(void* app);
    ISLEGAME_API void Application_Start(void* app);
    ISLEGAME_API void Application_Update(void* app);
    ISLEGAME_API void Application_Destroy(void* app);
    ISLEGAME_API void Application_SetEditorMode(void* app, bool mode);
    ISLEGAME_API void Application_SetPaused(void* app, bool paused);
}