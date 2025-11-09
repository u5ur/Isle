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

        if (!LoadGameDLL())
        {
            ISLE_ERROR("Failed to load game DLL on startup\n");
            return;
        }

        if (!InitializeGame())
        {
            ISLE_ERROR("Failed to initialize game on startup\n");
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

        m_Window->Clear(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        if (m_Game && m_SetPaused && m_UpdateFunc)
        {
            m_SetPaused(m_Game, m_Paused);
            m_UpdateFunc(m_Game);
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
        const char* dllPath = "build/IsleGame_Debug.dll";
#else
        const char* dllPath = "build/IsleGame.dll";
#endif

        ISLE_LOG("Loading game DLL: %s\n", dllPath);

        if (!ModuleManager::Instance()->Load(dllPath))
        {
            ISLE_ERROR("Failed to load: %s\n", dllPath);
            return false;
        }

        ISLE_LOG("Loaded %s\n", dllPath);

        m_CreateApp = ModuleManager::Instance()->Get<CreateAppFunc>("CreateApplication");
        m_DestroyApp = ModuleManager::Instance()->Get<DestroyAppFunc>("DestroyApplication");
        m_StartFunc = ModuleManager::Instance()->Get<StartFunc>("Application_Start");
        m_UpdateFunc = ModuleManager::Instance()->Get<UpdateFunc>("Application_Update");
        m_DestroyFunc = ModuleManager::Instance()->Get<DestroyFunc>("Application_Destroy");
        m_SetEditorMode = ModuleManager::Instance()->Get<SetEditorModeFunc>("Application_SetEditorMode");
        m_SetPaused = ModuleManager::Instance()->Get<SetPausedFunc>("Application_SetPaused");

        if (!m_CreateApp || !m_StartFunc || !m_UpdateFunc || !m_DestroyFunc || !m_SetEditorMode || !m_SetPaused)
        {
            ISLE_ERROR("Failed to load game API functions from DLL\n");
            return false;
        }

        return true;
    }

    void EditorApplication::UnloadGameDLL()
    {
        ModuleManager::Instance()->Unload();

        m_CreateApp = nullptr;
        m_DestroyApp = nullptr;
        m_StartFunc = nullptr;
        m_UpdateFunc = nullptr;
        m_DestroyFunc = nullptr;
        m_SetEditorMode = nullptr;
        m_SetPaused = nullptr;
    }

    bool EditorApplication::InitializeGame()
    {
        if (!m_CreateApp)
        {
            ISLE_ERROR("CreateApp function not loaded\n");
            return false;
        }

        Editor::Instance()->SetViewportTexture(nullptr);
        Render::Instance()->Reset();

        m_Game = m_CreateApp();
        if (!m_Game)
        {
            ISLE_ERROR("Failed to create game application\n");
            return false;
        }

        m_SetEditorMode(m_Game, true);
        Render::Instance()->SetEditorMode(true);

        m_StartFunc(m_Game);

        if (Scene::Instance())
        {
            Scene::Instance()->UploadToPipeline();
        }

        return true;
    }

    void EditorApplication::ShutdownGame()
    {
        Editor::Instance()->SetViewportTexture(nullptr);

        if (m_Game)
        {
            if (m_DestroyFunc)
                m_DestroyFunc(m_Game);
            if (m_DestroyApp)
                m_DestroyApp(m_Game);
            m_Game = nullptr;
        }

        Render::Instance()->SetEditorMode(false);
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