#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Structs/GpuStructs.h>

namespace Isle
{
	class Light : public SceneComponent
	{
	public:
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		bool m_CastShadows = true;
		float m_Intensity = 1.0f;
		glm::vec4 m_Color = glm::vec4(1.0f);

	public:
		Light();
		virtual ~Light() {};

		glm::mat4 GetViewProjectionMatrix();
		virtual GpuLight ToGpuLight();
	};

	class DirectionalLight : public Light
	{
	public:
		glm::vec3 m_Dir;

	public:
		DirectionalLight();
		glm::mat4 GetLightSpaceMatrix();
		virtual GpuLight ToGpuLight() override;
	};

	class PointLight : public Light
	{
	public:
		float m_Radius = 25.0f;
		glm::vec3 m_Position;
		glm::mat4 m_ShadowMatrices[6];

	public:
		PointLight();

		void UpdateMatrices();
		const glm::mat4* GetShadowMatrices() const { return m_ShadowMatrices; }
		virtual GpuLight ToGpuLight() override;
	};

	class SpotLight : public Light
	{
	public:
		glm::vec3 m_Position;
		glm::vec3 m_Direction;
		float m_Radius = 30.0f;
		float m_InnerCone = glm::radians(15.0f);
		float m_OuterCone = glm::radians(25.0f);

	public:
		SpotLight();

		glm::mat4 GetLightSpaceMatrix();
		void UpdateMatrices();
		virtual GpuLight ToGpuLight() override;
	};
}
