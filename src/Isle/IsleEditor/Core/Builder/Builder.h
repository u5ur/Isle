// Builder.h
#pragma once
#include <Core/Common/Common.h>
#include <string>

namespace Isle
{
    class Builder : public Singleton<Builder>, public Object
    {
    public:
#ifdef _DEBUG
        const char* m_DllName = "IsleGame_Debug.dll";
        const char* m_CopyDllName = "IsleGame_Debug_Copy.dll";
#else
        const char* m_DllName = "IsleGame.dll";
        const char* m_CopyDllName = "IsleGame_Copy.dll";
#endif
        const char* m_Path = "build/";

    public:
        bool BuildGame();
        bool RebuildGame();
        bool CleanGame();
        bool ReloadGame();

    private:
        bool RunCommand(const char* command);
        void CloseGame();
        void StartGame();
        void CopyGame();
        void DeleteGame();
    };
}
