// SceneComponent.h

namespace Isle
{
	class SceneComponent : public Component
	{
	public:
		SceneComponent* m_Owner = nullptr;
		std::vector<SceneComponent*> m_Children;
		Transform m_Transform = Transform();
		Bounds m_Bounds = Bounds();

	public:
		virtual void Update(float delta_time = 0.0f) {};

		void AddChild(SceneComponent* child)
		{
			if (!child || child == this)
				return;

			if (child->m_Owner)
				child->m_Owner->RemoveChild(child);

			child->m_Owner = this;
			m_Children.push_back(child);
		}

		void RemoveChild(SceneComponent* child)
		{
			if (!child)
				return;

			auto it = std::find(m_Children.begin(), m_Children.end(), child);
			if (it != m_Children.end())
			{
				child->m_Owner = nullptr;
				m_Children.erase(it);
			}
		}

		void RemoveAllChildren()
		{
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
			for (Component* child : m_Children)
			{
				if (T* casted = dynamic_cast<T*>(child))
					return casted;
			}
			return nullptr;
		}

		template<typename T>
		std::vector<T*> GetChildren()
		{
			std::vector<T*> results;
			for (SceneComponent* child : m_Children)
			{
				if (T* casted = dynamic_cast<T*>(child))
					results.push_back(casted);
			}
			return results;
		}

		std::vector<SceneComponent*> GetChildren()
		{
			std::vector<SceneComponent*> results;
			for (SceneComponent* child : m_Children)
			{
				results.push_back(child);
			}
			return results;
		}

		template<typename T>
		std::vector<T*> GetChildrenInChildren()
		{
			std::vector<T*> results;

			std::function<void(Component*)> traverse = [&](SceneComponent* current)
			{
				for (SceneComponent* child : current->m_Children)
				{
					if (T* casted = dynamic_cast<T*>(child))
						results.push_back(casted);

					traverse(child);
				}
			};

			traverse(this);
			return results;
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
			m_Transform = Transform::FromMatrix(matrix);
		}

		glm::mat4 GetLocalMatrix() const
		{
			return m_Transform.ToMatrix();
		}

		void SetLocalPosition(const glm::vec3& pos)
		{
			m_Transform.m_Translation = pos;
		}

		void SetLocalRotation(const glm::quat& rot)
		{
			m_Transform.m_Rotation = rot;
		}

		void SetLocalScale(const glm::vec3& scl)
		{
			m_Transform.m_Scale = scl;
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
			const auto Parent = m_Owner;
			if (Parent)
				return Parent->GetWorldMatrix() * GetLocalMatrix();
			return GetLocalMatrix();
		}

		void SetWorldMatrix(const glm::mat4& matrix)
		{
			const auto Parent = m_Owner;
			glm::mat4 localMatrix = matrix;
			if (Parent)
			{
				glm::mat4 parentWorld = Parent->GetWorldMatrix();
				glm::mat4 parentInv = glm::inverse(parentWorld);
				localMatrix = parentInv * matrix;
			}
			m_Transform = Transform::FromMatrix(localMatrix);
		}

		glm::vec3 GetWorldPosition() const
		{
			glm::mat4 world = GetWorldMatrix();
			return glm::vec3(world[3]);
		}

		void SetWorldPosition(const glm::vec3& pos)
		{
			const auto Parent = m_Owner;
			if (Parent)
			{
				glm::mat4 parentWorld = Parent->GetWorldMatrix();
				glm::mat4 parentInv = glm::inverse(parentWorld);
				glm::vec4 localPos = parentInv * glm::vec4(pos, 1.0f);
				m_Transform.m_Translation = glm::vec3(localPos);
			}
			else
			{
				m_Transform.m_Translation = pos;
			}
		}

		glm::quat GetWorldRotation() const
		{
			const auto Parent = m_Owner;
			if (Parent)
				return Parent->GetWorldRotation() * m_Transform.m_Rotation;
			return m_Transform.m_Rotation;
		}

		void SetWorldRotation(const glm::quat& rot)
		{
			const auto Parent = m_Owner;
			if (Parent)
			{
				glm::quat parentRot = Parent->GetWorldRotation();
				glm::quat invParent = glm::inverse(parentRot);
				m_Transform.m_Rotation = invParent * rot;
			}
			else
			{
				m_Transform.m_Rotation = rot;
			}
		}

		glm::vec3 GetWorldScale() const
		{
			const auto Parent = m_Owner;
			if (Parent)
				return Parent->GetWorldScale() * m_Transform.m_Scale;
			return m_Transform.m_Scale;
		}

		void SetWorldScale(const glm::vec3& scl)
		{
			const auto Parent = m_Owner;
			if (Parent)
			{
				glm::vec3 parentScale = Parent->GetWorldScale();
				m_Transform.m_Scale = scl / parentScale;
			}
			else
			{
				m_Transform.m_Scale = scl;
			}
		}
	};
}
