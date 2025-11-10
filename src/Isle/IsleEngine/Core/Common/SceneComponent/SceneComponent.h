namespace Isle
{
    class ISLEENGINE_API SceneComponent : public Component
    {
    public:
        SceneComponent* m_Owner = nullptr;
        std::vector<SceneComponent*> m_Children;
        Transform m_Transform = Transform();
        Bounds m_Bounds = Bounds();
        bool m_TransformDirty = true;

    private:
        bool m_IsDestroyed = false;

    public:
        virtual void Update(float delta_time = 0.0f) {};

        bool IsValid() const { return !m_IsDestroyed; }

        void AddChild(SceneComponent* child)
        {
            if (!child || child == this || m_IsDestroyed)
            {
                return;
            }

            if (std::find(m_Children.begin(), m_Children.end(), child) != m_Children.end())
            {
                return;
            }

            if (child->m_Owner && child->m_Owner != this)
            {
                child->m_Owner->RemoveChild(child);
            }

            child->m_Owner = this;
            m_Children.push_back(child);
        }

        void RemoveChild(SceneComponent* child)
        {
            if (!child || m_IsDestroyed)
                return;

            auto it = std::find(m_Children.begin(), m_Children.end(), child);
            if (it != m_Children.end())
            {
                child->m_Owner = nullptr;
                m_Children.erase(it);
            }
        }

        void SetChild(size_t index, SceneComponent* child, bool transferOwnership = true)
        {
            if (m_IsDestroyed)
                return;

            if (index >= m_Children.size())
            {
                return;
            }

            if (!child)
            {
                SceneComponent* oldChild = m_Children[index];
                if (oldChild && transferOwnership)
                    oldChild->m_Owner = nullptr;
                m_Children[index] = nullptr;
                return;
            }

            if (child == this)
            {
                return;
            }

            if (transferOwnership && child->m_Owner && child->m_Owner != this)
                child->m_Owner->RemoveChild(child);

            SceneComponent* oldChild = m_Children[index];
            if (oldChild && transferOwnership)
                oldChild->m_Owner = nullptr;

            if (transferOwnership)
                child->m_Owner = this;

            m_Children[index] = child;
        }

        virtual void Destroy() override
        {
            if (m_IsDestroyed)
                return;

            m_IsDestroyed = true;


            for (auto* child : m_Children)
            {
                if (child && child->IsValid())
                {
                    child->m_Owner = nullptr;
                }
            }

            m_Children.clear();
            m_Owner = nullptr;
        }

        void RemoveAllChildren()
        {
            if (m_IsDestroyed)
                return;

            for (auto* child : m_Children)
            {
                if (child)
                    child->m_Owner = nullptr;
            }
            m_Children.clear();
        }

        template<typename T>
        T* GetChild()
        {
            if (m_IsDestroyed)
                return nullptr;

            for (SceneComponent* child : m_Children)
            {
                if (!child || !child->IsValid())
                    continue;

                if (T* casted = dynamic_cast<T*>(child))
                    return casted;
            }
            return nullptr;
        }

        template<typename T>
        std::vector<T*> GetChildren()
        {
            std::vector<T*> results;

            if (m_IsDestroyed)
                return results;

            for (SceneComponent* child : m_Children)
            {
                if (!child || !child->IsValid())
                    continue;

                if (T* casted = dynamic_cast<T*>(child))
                    results.push_back(casted);
            }
            return results;
        }

        std::vector<SceneComponent*> GetChildren()
        {
            std::vector<SceneComponent*> results;

            if (m_IsDestroyed)
                return results;

            for (SceneComponent* child : m_Children)
            {
                if (child && child->IsValid())
                    results.push_back(child);
            }
            return results;
        }

        template<typename T>
        std::vector<T*> GetChildrenInChildren()
        {
            std::vector<T*> results;

            if (m_IsDestroyed)
                return results;

            std::function<void(SceneComponent*)> traverse = [&](SceneComponent* current)
                {
                    if (!current || !current->IsValid())
                        return;

                    for (SceneComponent* child : current->m_Children)
                    {
                        if (!child || !child->IsValid())
                            continue;

                        if (T* casted = dynamic_cast<T*>(child))
                            results.push_back(casted);

                        traverse(child);
                    }
                };

            traverse(this);
            return results;
        }

        std::vector<SceneComponent*> GetChildrenInChildren()
        {
            std::vector<SceneComponent*> results;

            if (m_IsDestroyed)
                return results;

            std::function<void(SceneComponent*)> traverse = [&](SceneComponent* current)
                {
                    if (!current || !current->IsValid())
                        return;

                    for (SceneComponent* child : current->m_Children)
                    {
                        if (!child || !child->IsValid())
                            continue;

                        results.push_back(child);
                        traverse(child);
                    }
                };

            traverse(this);
            return results;
        }

        void SetBounds(Bounds bounds)
        {
            if (!m_IsDestroyed)
                m_Bounds = bounds;
        }

        Bounds GetBounds() const
        {
            return m_Bounds;
        }

        size_t GetChildCount() const
        {
            return m_Children.size();
        }

        SceneComponent* GetParent() const
        {
            return m_Owner;
        }

        bool HasChildren() const
        {
            return !m_Children.empty();
        }

        bool HasParent() const
        {
            return m_Owner != nullptr;
        }

        void SetLocalMatrix(const glm::mat4& matrix)
        {
            if (!m_IsDestroyed)
                m_Transform = Transform::FromMatrix(matrix);
        }

        glm::mat4 GetLocalMatrix() const
        {
            return m_Transform.ToMatrix();
        }

        void SetLocalPosition(const glm::vec3& pos)
        {
            if (!m_IsDestroyed)
                m_Transform.m_Translation = pos;

            MarkDirty();
        }

        void SetLocalRotation(const glm::quat& rot)
        {
            if (!m_IsDestroyed)
                m_Transform.m_Rotation = rot;

            MarkDirty();
        }

        void SetLocalScale(const glm::vec3& scl)
        {
            if (!m_IsDestroyed)
                m_Transform.m_Scale = scl;

            MarkDirty();
        }

        glm::vec3 GetLocalPosition() const
        {
            return m_Transform.m_Translation;
        }

        glm::quat GetLocalRotation() const
        {
            return m_Transform.m_Rotation;
        }

        glm::vec3 GetLocalScale() const
        {
            return m_Transform.m_Scale;
        }

        glm::mat4 GetWorldMatrix() const
        {
            if (m_Owner && m_Owner->IsValid())
                return m_Owner->GetWorldMatrix() * GetLocalMatrix();
            return GetLocalMatrix();
        }

        void SetWorldMatrix(const glm::mat4& matrix)
        {
            if (m_IsDestroyed)
                return;

            glm::mat4 localMatrix = matrix;

            if (m_Owner && m_Owner->IsValid())
            {
                glm::mat4 parentWorld = m_Owner->GetWorldMatrix();
                glm::mat4 parentInv = glm::inverse(parentWorld);
                localMatrix = parentInv * matrix;
            }

            m_Transform = Transform::FromMatrix(localMatrix);
            MarkDirty();
        }

        glm::vec3 GetWorldPosition() const
        {
            glm::mat4 world = GetWorldMatrix();
            return glm::vec3(world[3]);
        }

        void SetWorldPosition(const glm::vec3& pos)
        {
            if (m_IsDestroyed)
                return;

            if (m_Owner && m_Owner->IsValid())
            {
                glm::mat4 parentWorld = m_Owner->GetWorldMatrix();
                glm::mat4 parentInv = glm::inverse(parentWorld);
                glm::vec4 localPos = parentInv * glm::vec4(pos, 1.0f);
                m_Transform.m_Translation = glm::vec3(localPos);
            }
            else
            {
                m_Transform.m_Translation = pos;
            }

            MarkDirty();
        }

        glm::quat GetWorldRotation() const
        {
            if (m_Owner && m_Owner->IsValid())
                return m_Owner->GetWorldRotation() * m_Transform.m_Rotation;
            return m_Transform.m_Rotation;
        }

        void SetWorldRotation(const glm::quat& rot)
        {
            if (m_IsDestroyed)
                return;

            if (m_Owner && m_Owner->IsValid())
            {
                glm::quat parentRot = m_Owner->GetWorldRotation();
                glm::quat invParent = glm::inverse(parentRot);
                m_Transform.m_Rotation = invParent * rot;
            }
            else
            {
                m_Transform.m_Rotation = rot;
            }

            MarkDirty();
        }

        glm::vec3 GetWorldScale() const
        {
            if (m_Owner && m_Owner->IsValid())
                return m_Owner->GetWorldScale() * m_Transform.m_Scale;
            return m_Transform.m_Scale;
        }

        void SetWorldScale(const glm::vec3& scl)
        {
            if (m_IsDestroyed)
                return;

            if (m_Owner && m_Owner->IsValid())
            {
                glm::vec3 parentScale = m_Owner->GetWorldScale();
                m_Transform.m_Scale = scl / parentScale;
            }
            else
            {
                m_Transform.m_Scale = scl;
            }

            MarkDirty();
        }

        bool IsTransformDirty()
        {
            return m_TransformDirty;
        }

        void MarkDirty(bool value = true)
        {
            m_TransformDirty = value;
        }
    };
}