// CameraMan.h
#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
	class CameraMan : public Singleton<CameraMan>, public SceneComponent
	{
	public:
		Camera* m_Camera;
		bool m_UseCameraMan = false;

	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;

		void SetCamera(Camera* camera);
	};
}