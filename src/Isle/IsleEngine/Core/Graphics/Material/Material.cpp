#include "Material.h"
#include "../Shader/Shader.h"
#include "Core/Graphics/Texture/Texture.h"

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

    GpuMaterial Material::GetGpuMaterial()
    {
        GpuMaterial GMaterial{};

        auto baseColorTex = GetTexture("BaseColor");
        auto emissiveTex = GetTexture("Emissive");
        auto metallicRoughnessTex = GetTexture("MetallicRoughness");
        auto occlusionTex = GetTexture("Occlusion");

        GMaterial.m_BaseColor_TexIndex = baseColorTex ? baseColorTex->m_BindlessIndex : -1;
        GMaterial.m_Emissive_TexIndex = emissiveTex ? emissiveTex->m_BindlessIndex : -1;
        GMaterial.m_MetallicRoughness_TexIndex = metallicRoughnessTex ? metallicRoughnessTex->m_BindlessIndex : -1;
        GMaterial.m_Occlusion_TexIndex = occlusionTex ? occlusionTex->m_BindlessIndex : -1;
        GMaterial.m_NormalScale = m_NormalScale;
        GMaterial.m_OcclusionStrength = m_OcclusionStrength;
        GMaterial.m_MetallicFactor = m_MetallicFactor;
        GMaterial.m_EmissiveFactor = m_EmissiveFactor;
        GMaterial.m_BaseColorFactor = m_BaseColorFactor;
        GMaterial.m_RoughnessFactor = m_RoughnessFactor;
        GMaterial.m_IOR = m_IOR;

        return GMaterial;
    }

    Ref<Texture> Material::GetTexture(const std::string& name)
    {
        auto it = m_Textures.find(name);
        return (it != m_Textures.end()) ? it->second : nullptr;
    }

    void Material::SetTexture(const std::string& name, Ref<Texture> texture)
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