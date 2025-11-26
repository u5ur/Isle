#include "EditorApplication.h"
#include <Core/ModuleManager/ModuleManager.h>
#include <Core/Editor/Editor.h>

namespace Isle
{
    void EditorApplication::Start()
    {
        m_Window = new Window();
        m_Window->Start();

        Input::Instance()->Start(m_Window);
        Input::Instance()->SetCursorMode(false);

        Render::Instance()->Start(m_Window);

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(m_Window->m_Handle, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        ImGui::StyleColorsDark();
        ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

        Editor::Instance()->Start();
        MainCamera::Instance()->Start();
        EditorCamera::Instance()->Start();

        ISLE_SUCCESS("Editor Loaded!\n");

        if (!LoadGameDLL())
        {
            ISLE_ERROR("Failed to load game DLL on startup\n");
            return;
        }

        if (!InitializeGame())
        {
            ISLE_ERROR("Failed to initialize game on startup\n");
        }
        else
        {
            ISLE_SUCCESS("Game Loaded!\n");
        }
    }

    void EditorApplication::Update()
    {
        if (auto mesh = dynamic_cast<Mesh*>(Editor::Instance()->GetSelectedComponent()))
        {
            Render::Instance()->GetPipeline()->SelectMesh(mesh, true);
        }

        m_Window->PollEvents();
        Input::Instance()->Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        static bool prevEsc = false;
        bool esc = Input::Instance()->IsKeyPressed(GLFW_KEY_ESCAPE);
        if (esc && !prevEsc && !m_Paused)
        {
            m_Paused = true;
            Input::Instance()->SetCursorMode(false);
        }
        prevEsc = esc;

        EditorCamera::Instance()->Update(0.0f);

        m_Window->Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        if (Application::Instance())
        {
            Application::Instance()->Update();
        }

        Editor::Instance()->Update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_Window->SwapBuffers();
        Input::Instance()->Reset();
    }

    void EditorApplication::Destroy()
    {
        ShutdownGame();
        UnloadGameDLL();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (m_Window)
        {
            m_Window->Destroy();
            delete m_Window;
            m_Window = nullptr;
        }

        ISLE_LOG("Editor closed\n");
    }

    bool EditorApplication::LoadGameDLL()
    {
#ifdef _DEBUG
        const char* dllPath = "build/Debug/IsleGame_Debug.dll";
#else
        const char* dllPath = "build/Build/IsleGame.dll";
#endif

        if (!ModuleManager::Instance()->Load(dllPath))
        {
            ISLE_ERROR("Failed to load: %s\n", dllPath);
            return false;
        }

        return true;
    }

    void EditorApplication::UnloadGameDLL()
    {
        ModuleManager::Instance()->Unload();
    }

    bool EditorApplication::InitializeGame()
    {
        Editor::Instance()->SetViewportTexture(nullptr);
        Render::Instance()->Reset();

        if (!Application::Instance())
        {
            ISLE_ERROR("Game DLL did not register an Application instance\n");
            return false;
        }

        Application::Instance()->Start();
        MainCamera::Instance()->SetCamera(EditorCamera::Instance()->m_Camera);
        return true;
    }

    void EditorApplication::ShutdownGame()
    {
        if (Application::Instance())
        {
            Editor::Instance()->SetViewportTexture(nullptr);
            Application::Instance()->Destroy();
            Application::s_Instance = nullptr;
        }
    }

    Window* EditorApplication::GetWindow()
    {
        return m_Window;
    }

    bool EditorApplication::IsPaused()
    {
        return m_Paused;
    }

    void EditorApplication::SetPaused(bool pause)
    {
        m_Paused = pause;
    }
}