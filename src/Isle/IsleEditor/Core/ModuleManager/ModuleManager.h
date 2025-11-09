// ModuleManager.h
#pragma once
#include <windows.h>
#include <IsleEngine.h>

namespace Isle
{
    template<typename T>
    T GetFunc(HMODULE dll, const char* name)
    {
        return reinterpret_cast<T>(GetProcAddress(dll, name));
    }

    class ModuleManager : public Singleton<ModuleManager>
    {
    private:
        HMODULE m_Handle = nullptr;

    public:
        bool Load(const char* path)
        {
            Unload();
            m_Handle = LoadLibraryA(path);
            return m_Handle != nullptr;
        }

        void Unload()
        {
            if (m_Handle)
            {
                FreeLibrary(m_Handle);
                m_Handle = nullptr;
            }
        }

        template<typename T>
        T Get(const char* funcName)
        {
            return m_Handle ? GetFunc<T>(m_Handle, funcName) : nullptr;
        }

        bool IsLoaded() const { return m_Handle != nullptr; }
    };
}

#define ISLE_FUNC(FuncType, FuncName) Isle::ModuleManager::Instance()->Get<FuncType>(FuncName)