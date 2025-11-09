// OrthographicCamera.h
#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
	class ISLEENGINE_API OrthographicCamera : public Singleton<OrthographicCamera>, public SceneComponent
	{
	public:
		Camera* m_Camera;

		float m_Left = -10.0f;
		float m_Right = 10.0f;
		float m_Bottom = -10.0f;
		float m_Top = 10.0f;

		glm::vec3 m_Target = glm::vec3(0.0f);
		float m_Distance = 50.0f;
		float m_Pitch = 30.0f;
		float m_Yaw = -45.0f;
		float m_ZoomLevel = 1.0f;

	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;

		void UpdateCameraPosition();
		void SetTarget(glm::vec3 target);
		void SetOrthographicBounds(float left, float right, float bottom, float top);
		void SetZoomLevel(float zoom);
	};
}