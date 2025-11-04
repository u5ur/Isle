#include "Light.h"

namespace Isle
{
    Light::Light()
    {
        m_Transform = Transform();
        m_ViewMatrix = glm::mat4(1.0f);
        m_ProjectionMatrix = glm::mat4(1.0f);
    }

    glm::mat4 Light::GetViewProjectionMatrix()
    {
        return m_ProjectionMatrix * m_ViewMatrix;
    }

    DirectionalLight::DirectionalLight()
    {
        m_Transform.SetTranslation(glm::vec3(0.0f, 50.0f, 0.0f));
        m_ViewMatrix = glm::mat4(1.0f);
        m_ProjectionMatrix = glm::mat4(1.0f);
        m_Dir = glm::normalize(glm::vec3(-0.1f, -1.0f, 0.15f));
        m_Color = glm::vec4(1, 0.5f, 0.4f, 1.0f);
        m_Intensity = 1.0f;
    }

    glm::mat4 DirectionalLight::GetLightSpaceMatrix()
    {
        float orthoSize = 20.0f;
        float nearPlane = 0.1f;
        float farPlane = 150.0f;

        glm::vec3 lightDir = glm::normalize(m_Dir);
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec3 lightPos = target - lightDir * 100.0f;

        m_ViewMatrix = glm::lookAt(lightPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
        m_ProjectionMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);

        return m_ProjectionMatrix * m_ViewMatrix;
    }

    PointLight::PointLight()
    {
        m_Position = glm::vec3(0.0f, 10.0f, 0.0f);
        m_Color = glm::vec4(1.0f);
        m_Intensity = 1.0f;
        m_Radius = 25.0f;
        UpdateMatrices();
    }

    void PointLight::UpdateMatrices()
    {
        float nearPlane = 0.1f;
        float farPlane = m_Radius;
        m_ProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

        m_ShadowMatrices[0] = m_ProjectionMatrix * glm::lookAt(m_Position, m_Position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
        m_ShadowMatrices[1] = m_ProjectionMatrix * glm::lookAt(m_Position, m_Position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
        m_ShadowMatrices[2] = m_ProjectionMatrix * glm::lookAt(m_Position, m_Position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        m_ShadowMatrices[3] = m_ProjectionMatrix * glm::lookAt(m_Position, m_Position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
        m_ShadowMatrices[4] = m_ProjectionMatrix * glm::lookAt(m_Position, m_Position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
        m_ShadowMatrices[5] = m_ProjectionMatrix * glm::lookAt(m_Position, m_Position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
    }

    SpotLight::SpotLight()
    {
        m_Position = glm::vec3(0.0f, 10.0f, 0.0f);
        m_Direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
        m_Color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f);
        m_Intensity = 2.0f;
        m_Radius = 30.0f;
        m_InnerCone = glm::radians(15.0f);
        m_OuterCone = glm::radians(25.0f);

        UpdateMatrices();
    }

    void SpotLight::UpdateMatrices()
    {
        float nearPlane = 0.1f;
        float farPlane = m_Radius;

        m_ProjectionMatrix = glm::perspective(m_OuterCone * 2.0f, 1.0f, nearPlane, farPlane);
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, glm::vec3(0, 1, 0));
    }

    glm::mat4 SpotLight::GetLightSpaceMatrix()
    {
        return m_ProjectionMatrix * m_ViewMatrix;
    }

    GpuLight Light::ToGpuLight()
    {
        GpuLight gpu{};
        gpu.m_Color = glm::vec3(m_Color);
        gpu.m_Intensity = m_Intensity;
        gpu.m_Type = -1;
        gpu.m_LightSpaceMatrix = glm::mat4(1.0f);
        for (int i = 0; i < 6; i++)
            gpu.m_ShadowMatrices[i] = glm::mat4(1.0f);
        return gpu;
    }

    GpuLight DirectionalLight::ToGpuLight()
    {
        GpuLight gpu{};
        gpu.m_Color = glm::vec3(m_Color);
        gpu.m_Intensity = m_Intensity;
        gpu.m_Direction = glm::normalize(m_Dir);
        gpu.m_Type = 0;
        gpu.m_LightSpaceMatrix = GetLightSpaceMatrix();
        return gpu;
    }

    GpuLight PointLight::ToGpuLight()
    {
        GpuLight gpu{};
        gpu.m_Color = glm::vec3(m_Color);
        gpu.m_Intensity = m_Intensity;
        gpu.m_Position = m_Position;
        gpu.m_Radius = m_Radius;
        gpu.m_Type = 1;
        for (int i = 0; i < 6; i++)
            gpu.m_ShadowMatrices[i] = m_ShadowMatrices[i];
        return gpu;
    }

    GpuLight SpotLight::ToGpuLight()
    {
        GpuLight gpu{};
        gpu.m_Color = glm::vec3(m_Color);
        gpu.m_Intensity = m_Intensity;
        gpu.m_Position = m_Position;
        gpu.m_Direction = glm::normalize(m_Direction);
        gpu.m_Radius = m_Radius;
        gpu.m_ConeAngles = glm::vec2(m_InnerCone, m_OuterCone);
        gpu.m_Type = 2;
        gpu.m_LightSpaceMatrix = GetLightSpaceMatrix();
        return gpu;
    }
}
