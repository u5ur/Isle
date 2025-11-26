#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Structs/GpuStructs.h>

namespace Isle
{
    class Texture;
    class PipelineState;
    class Shader;

    class Material : public Object
    {
    public:
        int m_Id = -1;
        Ref<Shader> m_Shader = nullptr;
        Ref<PipelineState> m_PipelineState = nullptr;
        std::map<std::string, Ref<Texture>> m_Textures;

    private:
        glm::vec4 m_BaseColorFactor = glm::vec4(1.0f);
        float m_MetallicFactor = 0.0f;
        float m_RoughnessFactor = 1.0f;
        glm::vec3 m_EmissiveFactor = glm::vec3(1.0f);
        float m_NormalScale = 1.0f;
        float m_OcclusionStrength = 1.0f;
        float m_EmissiveStrength = 1.0f;
        float m_IOR = 1.0f;
        bool m_Transparent = false;
        bool m_Dirty = true;

    public:
        Material();
        void Bind();
        GpuMaterial GetGpuMaterial();

        Ref<Texture> GetTexture(const std::string& name);
        void SetTexture(const std::string& name, Texture* texture);
        void SetShader(Ref<Shader> shader) { m_Shader = shader; }
        void SetPipelineState(Ref<PipelineState> state) { m_PipelineState = state; }

        void SetBaseColorFactor(const glm::vec4& value);
        void SetMetallicFactor(float value);
        void SetRoughnessFactor(float value);
        void SetEmissiveFactor(const glm::vec3& value);
        void SetNormalScale(float value);
        void SetOcclusionStrength(float value);
        void SetEmissiveStrength(float value);
        void SetIOR(float value);
        void SetTransparent(bool value);

        const glm::vec4& GetBaseColorFactor() const { return m_BaseColorFactor; }
        float GetMetallicFactor() const { return m_MetallicFactor; }
        float GetRoughnessFactor() const { return m_RoughnessFactor; }
        const glm::vec3& GetEmissiveFactor() const { return m_EmissiveFactor; }
        float GetNormalScale() const { return m_NormalScale; }
        float GetOcclusionStrength() const { return m_OcclusionStrength; }
        float GetEmissiveStrength() const { return m_EmissiveStrength; }
        float GetIOR() const { return m_IOR; }
        bool GetTransparent() const { return m_Transparent; }

        bool IsDirty();
        void MarkDirty(bool value = true);
    };
}
