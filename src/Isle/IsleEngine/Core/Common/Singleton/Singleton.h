#pragma once

namespace Isle
{
    template<typename T>
    class Singleton
    {
    private:
        static T* s_Instance;
        static bool s_IsValid;

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;

    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        static T* Instance()
        {
            if (!s_Instance)
            {
                s_Instance = new T();
                s_IsValid = true;
            }
            return s_Instance;
        }

        static void ResetInstance()
        {
            if (s_Instance && s_IsValid)
            {
                delete s_Instance;
                s_Instance = nullptr;
                s_IsValid = false;
            }
        }

        static bool IsValid() { return s_IsValid; }
    };

    template<typename T>
    T* Singleton<T>::s_Instance = nullptr;

    template<typename T>
    bool Singleton<T>::s_IsValid = false;
}
