#include "Material.h"
#include "../Shader/Shader.h"
#include "Core/Graphics/Texture/Texture.h"

namespace Isle
{
    Material::Material()
    {

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
        auto normalTex = GetTexture("Normal");

        GMaterial.m_BaseColor_TexIndex = baseColorTex ? baseColorTex->m_BindlessIndex : -1;
        GMaterial.m_Emissive_TexIndex = emissiveTex ? emissiveTex->m_BindlessIndex : -1;
        GMaterial.m_MetallicRoughness_TexIndex = metallicRoughnessTex ? metallicRoughnessTex->m_BindlessIndex : -1;
        GMaterial.m_Occlusion_TexIndex = occlusionTex ? occlusionTex->m_BindlessIndex : -1;
        GMaterial.m_Normal_TexIndex = normalTex ? normalTex->m_BindlessIndex : -1;

        GMaterial.m_NormalScale = m_NormalScale;
        GMaterial.m_OcclusionStrength = m_OcclusionStrength;
        GMaterial.m_MetallicFactor = m_MetallicFactor;
        GMaterial.m_EmissiveFactor = m_EmissiveFactor * m_EmissiveStrength;
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

    void Material::SetTexture(const std::string& name, Texture* texture)
    {
        m_Textures[name] = Ref<Texture>(texture);
        MarkDirty();
    }

    void Material::SetBaseColorFactor(const glm::vec4& value)
    {
        if (m_BaseColorFactor != value)
        {
            m_BaseColorFactor = value;
            MarkDirty();
        }
    }

    void Material::SetMetallicFactor(float value)
    {
        if (m_MetallicFactor != value)
        {
            m_MetallicFactor = value;
            MarkDirty();
        }
    }

    void Material::SetRoughnessFactor(float value)
    {
        if (m_RoughnessFactor != value)
        {
            m_RoughnessFactor = value;
            MarkDirty();
        }
    }

    void Material::SetEmissiveFactor(const glm::vec3& value)
    {
        if (m_EmissiveFactor != value)
        {
            m_EmissiveFactor = value;
            MarkDirty();
        }
    }

    void Material::SetNormalScale(float value)
    {
        if (m_NormalScale != value)
        {
            m_NormalScale = value;
            MarkDirty();
        }
    }

    void Material::SetOcclusionStrength(float value)
    {
        if (m_OcclusionStrength != value)
        {
            m_OcclusionStrength = value;
            MarkDirty();
        }
    }

    void Material::SetEmissiveStrength(float value)
    {
        if (m_EmissiveStrength != value)
        {
            m_EmissiveStrength = value;
            MarkDirty();
        }
    }

    void Material::SetTransparent(bool value)
    {
        if (m_Transparent != value)
        {
            m_Transparent = value;
            MarkDirty();
        }
    }

    void Material::SetIOR(float value)
    {
        if (m_IOR != value)
        {
            m_IOR = value;
            MarkDirty();
        }
    }

    bool Material::IsDirty()
    {
        return m_Dirty;
    }
        
    void Material::MarkDirty(bool value)
    {
        m_Dirty = value;
    }
}