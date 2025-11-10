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
        bool m_Paused = false;

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
