#pragma once
#include <windows.h>
#include <IsleEngine.h>
#include <IsleGame.h>

#if !defined(_WIN32)
#error "IsleDynamic is currently only supported on Windows."
#endif

#define DYNLIB_HANDLE       HMODULE
#define DYNLIB_LOAD(path)   LoadLibraryA(path)
#define DYNLIB_GETSYM(h, s) GetProcAddress(h, s)
#define DYNLIB_FREE(h)      FreeLibrary(h)

#define LOAD_LIBRARY(var, name)                                               \
    DYNLIB_HANDLE var = DYNLIB_LOAD(name);                                    \
    if (!(var)) {                                                             \
        ISLE_ERROR("Failed to load dynamic library: %s\n", name);             \
        return -1;                                                            \
    }

#define LOAD_FUNCTION(var, handle, name, type)                                \
    type var = reinterpret_cast<type>(DYNLIB_GETSYM(handle, #name));          \
    if (!(var)) {                                                             \
        ISLE_ERROR("Failed to locate symbol: %s in loaded module.\n", #name); \
        DYNLIB_FREE(handle);                                                  \
        return -1;                                                            \
    }

typedef Isle::Application* (*CreateApplicationFn)();

int main()
{
#ifdef _DEBUG
    const char* Dll_Name = "IsleGame_Debug.dll";
#else
    const char* Dll_Name = "IsleGame.dll";
#endif

    LOAD_LIBRARY(Game_Handle, Dll_Name);
    LOAD_FUNCTION(CreateApplication, Game_Handle, CreateApplication, CreateApplicationFn);

    Isle::Application* Application = CreateApplication();
    Application->Start();

    while (true)
    {
        Application->Update();
    }

    Application->Destroy();
    DYNLIB_FREE(Game_Handle);
    return 0;
}