// Ref.h
#pragma once

namespace Isle
{
    template<typename T>
    class Ref
    {
    private:
        std::shared_ptr<T> m_Ptr;

    public:
        Ref() = default;
        Ref(std::nullptr_t) {}
        Ref(std::shared_ptr<T> ptr) : m_Ptr(std::move(ptr)) {}

        template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
        Ref(const Ref<U>& other) : m_Ptr(other.m_Ptr) {}

        T* operator->() const { return m_Ptr.get(); }
        T& operator*() const { return *m_Ptr; }
        T* Get() const { return m_Ptr.get(); }

        explicit operator bool() const { return m_Ptr != nullptr; }
        bool IsValid() const { return m_Ptr != nullptr; }

        template<typename U>
        Ref<U> As() const
        {
            return Ref<U>(std::dynamic_pointer_cast<U>(m_Ptr));
        }

        void Reset() { m_Ptr.reset(); }

        bool operator==(const Ref& other) const { return m_Ptr == other.m_Ptr; }
        bool operator!=(const Ref& other) const { return m_Ptr != other.m_Ptr; }
        bool operator==(std::nullptr_t) const { return !m_Ptr; }
        bool operator!=(std::nullptr_t) const { return m_Ptr != nullptr; }
    };
}