// Builder.cpp
#include "Builder.h"
#include <Core/EditorApplication/EditorApplication.h>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

namespace Isle
{
    bool Builder::RunCommand(const char* command)
    {
        int result = system(command);
        if (result != 0)
        {
            return false;
        }
        return true;
    }

    void Builder::CloseGame()
    {
        auto app = EditorApplication::Instance();
        if (!app) return;

        app->ShutdownGame();
        app->UnloadGameDLL();
        Sleep(1000);
    }

    void Builder::StartGame()
    {
        auto app = EditorApplication::Instance();
        if (!app) 
            return;

        if (!app->LoadGameDLL())
        {
            ISLE_ERROR("Failed to load game DLL!\n");
            return;
        }

        if (!app->InitializeGame())
        {
            ISLE_ERROR("Failed to initialize game!\n");
        }
    }

    void Builder::CopyGame()
    {
        fs::path buildPath = fs::path(m_Path) / m_DllName;
        fs::path copyPath = fs::path(m_Path) / m_CopyDllName;

        if (fs::exists(copyPath))
            fs::remove(copyPath);

        if (fs::exists(buildPath))
        {
            fs::copy_file(buildPath, copyPath, fs::copy_options::overwrite_existing);
            ISLE_LOG("Copied %s %s\n", m_DllName, m_CopyDllName);
        }
    }

    void Builder::DeleteGame()
    {
        fs::path dllPath = fs::path(m_Path) / m_DllName;
        if (fs::exists(dllPath))
        {
            try
            {
                fs::remove(dllPath);
            }
            catch (const std::exception& e)
            {
                ISLE_ERROR("Failed to delete DLL: %s\n", e.what());
            }
        }
    }

    bool Builder::BuildGame()
    {
        return RunCommand("cmake --build --preset build-debug --target IsleGame >nul 2>&1");
    }

    bool Builder::RebuildGame()
    {
        if (!RunCommand("cmake --build --preset build-debug --target clean >nul 2>&1"))
            return false;
        return RunCommand("cmake --build --preset build-debug --target IsleGame >nul 2>&1");
    }

    bool Builder::CleanGame()
    {
        return RunCommand("cmake --build --preset build-debug --target clean >nul 2>&1");
    }

    bool Builder::ReloadGame()
    {
        CloseGame();
        DeleteGame();

        if (!BuildGame())
        {
            ISLE_ERROR("Build failed. Attempting to recover...\n");
            StartGame();
            return false;
        }

        CopyGame();
        StartGame();

        ISLE_SUCCESS("Hot-Loaded Game!\n");
        return true;
    }
}
