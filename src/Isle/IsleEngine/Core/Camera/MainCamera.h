// MainCamera.h
#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
	class ISLEENGINE_API MainCamera : public Singleton<MainCamera>, public SceneComponent
	{
	public:
		Camera* m_CurrentCamera = nullptr;

	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;

		void SetCamera(Camera* camera);
		Camera* GetCamera();
	};
}