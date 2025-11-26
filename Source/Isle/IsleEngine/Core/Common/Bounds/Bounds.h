#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cfloat>

namespace Isle
{
	class Bounds
	{
	public:
		glm::vec3 m_Min = glm::vec3(FLT_MAX);
		glm::vec3 m_Max = glm::vec3(-FLT_MAX);

	public:
		Bounds() = default;
		Bounds(const glm::vec3& min, const glm::vec3& max)
			: m_Min(min), m_Max(max) {
		}

		bool IsValid() const
		{
			if (m_Min == glm::vec3(FLT_MAX) && m_Max == glm::vec3(-FLT_MAX))
				return false;

			return (m_Min.x <= m_Max.x && m_Min.y <= m_Max.y && m_Min.z <= m_Max.z);
		}

		void Reset()
		{
			m_Min = glm::vec3(FLT_MAX);
			m_Max = glm::vec3(-FLT_MAX);
		}

		void Encapsulate(const glm::vec3& point)
		{
			m_Min = glm::min(m_Min, point);
			m_Max = glm::max(m_Max, point);
		}

		void Encapsulate(const Bounds& other)
		{
			if (!other.IsValid()) return;
			m_Min = glm::min(m_Min, other.m_Min);
			m_Max = glm::max(m_Max, other.m_Max);
		}

		glm::vec3 GetCenter() const
		{
			return IsValid() ? (m_Min + m_Max) * 0.5f : glm::vec3(0.0f);
		}

		glm::vec3 GetSize() const
		{
			return IsValid() ? (m_Max - m_Min) : glm::vec3(0.0f);
		}

		glm::vec3 GetExtents() const
		{
			return GetSize() * 0.5f;
		}

		float GetDiagonal() const
		{
			return IsValid() ? glm::length(m_Max - m_Min) : 0.0f;
		}

		void Expand(float amount)
		{
			if (!IsValid()) return;
			glm::vec3 delta(amount);
			m_Min -= delta;
			m_Max += delta;
		}

		bool Contains(const glm::vec3& point) const
		{
			if (!IsValid())
				return false;

			return all(glm::greaterThanEqual(point, m_Min)) &&
				all(glm::lessThanEqual(point, m_Max));
		}

		bool Intersects(const Bounds& other) const
		{
			if (!IsValid() || !other.IsValid())
				return false;

			return (m_Min.x <= other.m_Max.x && m_Max.x >= other.m_Min.x) &&
				(m_Min.y <= other.m_Max.y && m_Max.y >= other.m_Min.y) &&
				(m_Min.z <= other.m_Max.z && m_Max.z >= other.m_Min.z);
		}

		Bounds Transform(const glm::mat4& matrix) const
		{
			if (!IsValid())
				return *this;

			glm::vec3 corners[8] = {
				{m_Min.x, m_Min.y, m_Min.z},
				{m_Max.x, m_Min.y, m_Min.z},
				{m_Max.x, m_Max.y, m_Min.z},
				{m_Min.x, m_Max.y, m_Min.z},
				{m_Min.x, m_Min.y, m_Max.z},
				{m_Max.x, m_Min.y, m_Max.z},
				{m_Max.x, m_Max.y, m_Max.z},
				{m_Min.x, m_Max.y, m_Max.z}
			};

			Bounds transformed;
			for (const auto& c : corners)
			{
				glm::vec3 world = glm::vec3(matrix * glm::vec4(c, 1.0f));
				transformed.Encapsulate(world);
			}
			return transformed;
		}
	};
}
#pragma once