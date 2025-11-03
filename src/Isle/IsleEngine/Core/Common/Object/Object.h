#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <shared_mutex>

namespace Isle
{
    class Object : public std::enable_shared_from_this<Object>
    {
    private:
        inline static std::atomic<int> s_NextId{ 1 };
        inline static std::unordered_map<int, std::weak_ptr<Object>> s_Registry;
        inline static std::shared_mutex s_RegistryMutex;

        const int m_Id;
        std::string m_Name;

        void AutoRegister()
        {
            std::unique_lock<std::shared_mutex> lock(s_RegistryMutex);
            s_Registry[m_Id] = weak_from_this();
        }

        void AutoUnregister()
        {
            std::unique_lock<std::shared_mutex> lock(s_RegistryMutex);
            s_Registry.erase(m_Id);
        }

    protected:
        Object(std::string name = "")
            : m_Id(s_NextId.fetch_add(1, std::memory_order_relaxed))
            , m_Name(std::move(name))
        {
        }

    public:
        virtual ~Object() { AutoUnregister(); }

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;
        Object(Object&&) = default;
        Object& operator=(Object&&) = default;

        int GetId() const { return m_Id; }
        const std::string& GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = std::move(name); }

        virtual std::string ToString() const
        {
            return m_Name.empty() ? "Object#" + std::to_string(m_Id) : m_Name;
        }

        template<typename T = Object>
        static std::shared_ptr<T> Find(int id)
        {
            std::shared_lock<std::shared_mutex> lock(s_RegistryMutex);
            auto it = s_Registry.find(id);
            if (it != s_Registry.end())
            {
                if (auto ptr = it->second.lock())
                    return std::dynamic_pointer_cast<T>(ptr);
            }
            return nullptr;
        }

        template<typename T>
        std::shared_ptr<T> As()
        {
            return std::dynamic_pointer_cast<T>(shared_from_this());
        }
    };

}