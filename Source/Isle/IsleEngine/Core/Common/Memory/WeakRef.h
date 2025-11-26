// WeakRef.h
#pragma once

namespace Isle
{
   template<typename T>
    class WeakRef
    {
    private:
        int m_Id = 0;
        std::weak_ptr<T> m_Weak;

    public:
        WeakRef() = default;

        WeakRef(const Ref<T>& ref)
        {
            if (ref)
            {
                m_Id = ref->GetId();
                m_Weak = ref.m_Ptr;
            }
        }

        Ref<T> Lock() const
        {
            if (auto ptr = m_Weak.lock())
                return Ref<T>(ptr);

            if (m_Id > 0)
                return Ref<T>(Object::Find<T>(m_Id));

            return Ref<T>();
        }

        bool IsValid() const { return !m_Weak.expired(); }
        int GetId() const { return m_Id; }
    };
}