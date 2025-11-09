// MainCamera.h
#pragma once
#include <Core/Common/Common.h>
#include "Camera.h"

namespace Isle
{
	enum CAMERA_TYPE : uint8_t
	{
		ORTHOGRAPHIC,
		DEFAULT_PERSPECTIVE,
		EDITOR_PERSPECTIVE
	};


	class ISLEENGINE_API MainCamera : public Singleton<MainCamera>, public SceneComponent
	{
	public:
		CAMERA_TYPE m_Type = CAMERA_TYPE::ORTHOGRAPHIC;

	public:
		virtual void Start() override;
		virtual void Update(float delta_time) override;

		void SetCamera(Camera* camera);
		Camera* GetCamera();
	};
}