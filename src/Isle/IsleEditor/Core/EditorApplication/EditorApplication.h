// EditorApplication.h
#pragma once
#include <IsleEngine.h>
#include <Core/Common/EditorCommon.h>
#include <Core/Graphics/Window/Window.h>
#include <Core/Graphics/FrameBuffer/FrameBuffer.h>

namespace Isle
{
    class EditorApplication : public Singleton<EditorApplication>, public Object
    {
    public:
        Window* m_Window = nullptr;
        void* m_Game = nullptr;
        bool m_Paused = false;

        typedef void* (*CreateAppFunc)();
        typedef void (*DestroyAppFunc)(void*);
        typedef void (*StartFunc)(void*);
        typedef void (*UpdateFunc)(void*);
        typedef void (*DestroyFunc)(void*);
        typedef void (*SetEditorModeFunc)(void*, bool);
        typedef void (*SetPausedFunc)(void*, bool);

        CreateAppFunc m_CreateApp = nullptr;
        DestroyAppFunc m_DestroyApp = nullptr;
        StartFunc m_StartFunc = nullptr;
        UpdateFunc m_UpdateFunc = nullptr;
        DestroyFunc m_DestroyFunc = nullptr;
        SetEditorModeFunc m_SetEditorMode = nullptr;
        SetPausedFunc m_SetPaused = nullptr;

    public:
        void Start();
        void Update();
        void Destroy();
        bool IsPaused();
        void SetPaused(bool pause);
        Window* GetWindow();

        bool LoadGameDLL();
        void UnloadGameDLL();
        bool InitializeGame();
        void ShutdownGame();
    };
}
