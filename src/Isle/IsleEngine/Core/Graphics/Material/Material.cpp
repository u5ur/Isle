#include "Material.h"
#include "../Shader/Shader.h"

namespace Isle
{
    static int m_NumMaterials = 0;

    Material::Material()
    {
        m_NumMaterials++;
        m_Version = m_NumMaterials;
    }

    void Material::Bind()
    {
        if (m_Shader)
            m_Shader->Bind();
    }

    Texture* Material::GetTexture(std::string name)
    {
        return m_Textures[name];
    }

    void Material::SetTexture(std::string name, Texture* texture)
    {
        m_Textures[name] = texture;
        m_Version++;
    }

    void Material::SetBaseColorFactor(const glm::vec4& value)
    {
        if (m_BaseColorFactor != value)
        {
            m_BaseColorFactor = value;
            m_Version++;
        }
    }

    void Material::SetMetallicFactor(float value)
    {
        if (m_MetallicFactor != value)
        {
            m_MetallicFactor = value;
            m_Version++;
        }
    }

    void Material::SetRoughnessFactor(float value)
    {
        if (m_RoughnessFactor != value)
        {
            m_RoughnessFactor = value;
            m_Version++;
        }
    }

    void Material::SetEmissiveFactor(const glm::vec3& value)
    {
        if (m_EmissiveFactor != value)
        {
            m_EmissiveFactor = value;
            m_Version++;
        }
    }

    void Material::SetNormalScale(float value)
    {
        if (m_NormalScale != value)
        {
            m_NormalScale = value;
            m_Version++;
        }
    }

    void Material::SetOcclusionStrength(float value)
    {
        if (m_OcclusionStrength != value)
        {
            m_OcclusionStrength = value;
            m_Version++;
        }
    }

    void Material::SetEmissiveStrength(float value)
    {
        if (m_EmissiveStrength != value)
        {
            m_EmissiveStrength = value;
            m_Version++;
        }
    }

    void Material::SetTransparent(bool value)
    {
        if (m_Transparent != value)
        {
            m_Transparent = value;
            m_Version++;
        }
    }

    void Material::SetIOR(float value)
    {
        if (m_IOR != value)
        {
            m_IOR = value;
            m_Version++;
        }
    }
}
