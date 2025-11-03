// Camera.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Structs/GpuStructs.h>

namespace Isle
{
	class Camera : public SceneComponent
	{
	public:
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;

		float m_Fov;
		float m_AspectRatio;
		float m_Near;
		float m_Far;
		float m_Pitch;
		float m_Yaw;

	public:
		Camera();

		GpuCamera GetCpuCamera();

		void SetLook(glm::vec3 target);
		void SetFov(float fov);
		void SetAspectRatio(float ratio);
		void SetNear(float fnear);
		void SetFar(float ffar);
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::vec3 rotation);

		void Move(glm::vec3 offset);
		void Rotate(float pitch, float yaw);

		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

	protected:
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateVectors();
	};

}