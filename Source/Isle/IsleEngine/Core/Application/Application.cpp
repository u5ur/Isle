#include "Application.h"

namespace Isle
{
    Application* Singleton<Application>::s_Instance = nullptr;
    bool Singleton<Application>::s_IsValid = false;

    Application* Singleton<Application>::Instance()
    {
        return s_Instance;
    }

    void Singleton<Application>::SetInstance(Application* instance)
    {
        if (!s_Instance)
        {
            s_Instance = instance;
            s_IsValid = true;
        }
    }

    void Singleton<Application>::ResetInstance()
    {
        if (s_Instance && s_IsValid)
        {
            delete s_Instance;
            s_Instance = nullptr;
            s_IsValid = false;
        }
    }

    bool Singleton<Application>::IsValid()
    {
        return s_IsValid;
    }
}