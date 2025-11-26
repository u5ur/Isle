// IsleGame.h
#pragma once
#include <IsleEngine.h>

namespace Isle
{
    class GameApplication : public Application
    {
    public:
        GameApplication();
        void Start() override;
        void Update() override;
        void Destroy() override;
    };
}
