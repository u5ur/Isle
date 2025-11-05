// MainCamera.h
#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
	class MainCamera : public Singleton<MainCamera>, public SceneComponent
	{
	public:
		bool m_UseCameraMan = false;

	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;

		void SetCamera(Camera* camera);
		Camera* GetCamera();
	};
}