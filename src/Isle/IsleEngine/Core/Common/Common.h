#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define NOMINMAX

#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>
#include <chrono>
#ifdef _WIN32
#ifdef ISLEENGINE_EXPORTS
#define ISLEENGINE_API __declspec(dllexport)
#else
#define ISLEENGINE_API __declspec(dllimport)
#endif
#else
#define ISLEENGINE_API
#endif

#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Object/Object.h"
#include "Memory/Ref.h"
#include "Memory/WeakRef.h"
#include "Component/Component.h"
#include "Singleton/Singleton.h"
#include "Transform/Transform.h"
#include "Bounds/Bounds.h"
#include "SceneComponent/SceneComponent.h"

#define ISLE_OBJECT_CLASS(ClassName) \
    friend class Isle::Object; \
    private: \
        static inline std::atomic<int> s_NextId{1}; \
        static inline std::unordered_map<int, std::weak_ptr<Isle::Object>> s_Registry; \
        static inline std::shared_mutex s_RegistryMutex; \
    public:

namespace Isle
{
    template<typename T, typename... Args>
    static Ref<T> CreateObject(Args&&... args)
    {
        auto obj = std::make_shared<T>(std::forward<Args>(args)...);
        obj->AutoRegister();
        return Ref<T>(obj);
    }

    template<typename T, typename... Args>
    static Ref<T> New(Args&&... args)
    {
        return CreateObject<T>(std::forward<Args>(args)...);
    }
}

#if defined(_WIN32)
#include <windows.h>
inline void EnableAnsiColors()
{
    static bool enabled = false;
    if (enabled)
        return;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    enabled = true;
}
#else
inline void EnableAnsiColors() {}
#endif

#define COLOR_RESET   "\033[0m"
#define COLOR_LOG     "\033[36m"
#define COLOR_ERROR   "\033[31m"
#define COLOR_WARN    "\033[33m"
#define COLOR_SUCCESS "\033[32m"

inline bool HasAnsiSupport()
{
#if defined(_WIN32)
    return true;
#else
    const char* term = getenv("TERM");
    return term && std::string(term).find("xterm") != std::string::npos;
#endif
}

#ifdef _DEBUG
#undef ERROR
#define ISLE_LOG(format, ...)     do { EnableAnsiColors(); if (HasAnsiSupport()) printf(COLOR_LOG "[LOG] " format COLOR_RESET, ##__VA_ARGS__); else printf("[LOG] " format, ##__VA_ARGS__); } while (0)
#define ISLE_ERROR(format, ...)   do { EnableAnsiColors(); if (HasAnsiSupport()) printf(COLOR_ERROR "[ERROR] " format COLOR_RESET, ##__VA_ARGS__); else printf("[ERROR] " format, ##__VA_ARGS__); } while (0)
#define ISLE_WARN(format, ...)    do { EnableAnsiColors(); if (HasAnsiSupport()) printf(COLOR_WARN "[WARN] " format COLOR_RESET, ##__VA_ARGS__); else printf("[WARN] " format, ##__VA_ARGS__); } while (0)
#define ISLE_SUCCESS(format, ...) do { EnableAnsiColors(); if (HasAnsiSupport()) printf(COLOR_SUCCESS "[SUCCESS] " format COLOR_RESET, ##__VA_ARGS__); else printf("[SUCCESS] " format, ##__VA_ARGS__); } while (0)
#else
#define ISLE_LOG(format, ...)     ((void)0)
#define ISLE_ERROR(format, ...)   ((void)0)
#define ISLE_WARN(format, ...)    ((void)0)
#define ISLE_SUCCESS(format, ...) ((void)0)
#endif


#define READ_FILE(path, outStr)                            \
    do {                                                   \
        std::ifstream _file(path);                         \
        if (!_file.is_open()) {                            \
            outStr = "";                                   \
        } else {                                           \
            std::stringstream _ss;                         \
            _ss << _file.rdbuf();                          \
            outStr = _ss.str();                            \
        }                                                  \
    } while (0)

#include "Reflection/Reflection.h"
