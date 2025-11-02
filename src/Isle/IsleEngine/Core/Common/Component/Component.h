// Component.h

#pragma once

namespace Isle
{
	class Component : public Object
	{
	public:
		Component* m_Owner = nullptr;
		std::vector<Component*> m_Children;

	public:
		virtual void Start() {};
		virtual void Update() {};
		virtual void Destroy()
		{
			for (auto* child : m_Children)
			{
				if (child)
					child->Destroy();
			}
			m_Children.clear();
		};

		void AddChild(Component* child)
		{
			if (!child || child == this)
				return;

			if (child->m_Owner)
				child->m_Owner->RemoveChild(child);

			child->m_Owner = this;
			m_Children.push_back(child);
		}

		void RemoveChild(Component* child)
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
			for (Component* child : m_Children)
			{
				if (T* casted = dynamic_cast<T*>(child))
					results.push_back(casted);
			}
			return results;
		}

		std::vector<Component*> GetChildren()
		{
			std::vector<Component*> results;
			for (Component* child : m_Children)
			{
				results.push_back(child);
			}
			return results;
		}

		template<typename T>
		std::vector<T*> GetChildrenInChildren()
		{
			std::vector<T*> results;

			std::function<void(Component*)> traverse = [&](Component* current)
				{
					for (Component* child : current->m_Children)
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

		Component* GetParent() const
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
	};
}