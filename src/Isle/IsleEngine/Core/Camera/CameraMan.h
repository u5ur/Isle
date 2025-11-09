// CameraMan.h
#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
	class ISLEENGINE_API CameraMan : public Singleton<CameraMan>, public SceneComponent
	{
	public:
		Camera* m_Camera;

	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;
	};
}