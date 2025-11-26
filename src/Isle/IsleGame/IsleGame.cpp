// IsleGame.cpp
#include "IsleGame.h"

namespace Isle
{
    GameApplication::GameApplication()
    {
        SetInstance(this);
    }

    void GameApplication::Start()
    {
        Scene::Instance()->Add(MainCamera::Instance());
        Scene::Instance()->Add(World::Instance());
        Engine::Instance()->Start();
    }
        
    void GameApplication::Update()
    {
        Engine::Instance()->Update();
    }

    void GameApplication::Destroy()
    {
        Engine::Instance()->Destroy();
    }
}

namespace Isle
{
    static GameApplication g_GameApplication;
}


