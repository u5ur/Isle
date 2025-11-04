// GPUStructs.h
#pragma once
#include <Core/Common/Common.h>


namespace Isle
{
    struct alignas(16) GpuVertex
    {
        glm::vec3 m_Position;
        float _pad0;
        glm::vec3 m_Normal;
        float _pad1;
        glm::vec2 m_TexCoord;
        glm::vec2 _pad2;
        glm::vec3 m_Tangent;
        float _pad3;
        glm::vec3 m_BitTangent;
        float _pad4;
        glm::vec4 m_Color;

        GpuVertex() = default;
        GpuVertex(
            const glm::vec3& position,
            const glm::vec3& normal,
            const glm::vec2& texCoord,
            const glm::vec3& tangent,
            const glm::vec3& bitTangent,
            const glm::vec4& color)
            : m_Position(position),
            _pad0(0.0f),
            m_Normal(normal),
            _pad1(0.0f),
            m_TexCoord(texCoord),
            _pad2(0.0f),
            m_Tangent(tangent),
            _pad3(0.0f),
            m_BitTangent(bitTangent),
            _pad4(0.0f),
            m_Color(color)
        {
        }
    };

    struct alignas(16) GpuSkinnedVertex
    {
        glm::vec3 m_Position;
        float _pad0;
        glm::vec3 m_Normal;
        float _pad1;
        glm::vec2 m_TexCoord;
        glm::vec2 _pad2;
        glm::vec3 m_Tangent;
        float _pad3;
        glm::vec3 m_BitTangent;
        float _pad4;
        glm::vec4 m_Color;
        glm::ivec4 m_BoneIds;
        glm::vec4 m_BoneWeights;
    };

    struct alignas(16) GpuMaterial
    {
        int m_BaseColor_TexIndex;
        int m_Normal_TexIndex;
        int m_MetallicRoughness_TexIndex;
        int m_Occlusion_TexIndex;

        int m_Emissive_TexIndex;
        int _pad0[3];

        glm::vec4 m_BaseColorFactor;
        glm::vec3 m_EmissiveFactor;
        float m_IOR;
        float m_MetallicFactor;
        float m_RoughnessFactor;
        float m_NormalScale;
        float m_OcclusionStrength;
        int m_Transparent;
        float _pad2[2];
    };

    struct alignas(16) GpuStaticMesh
    {
        glm::mat4 m_Transform;
        glm::mat4 m_NormalMatrix;
        glm::vec3 m_AABBMin;
        float _pad0;
        glm::vec3 m_AABBMax;
        int m_Selected;
        uint32_t m_VertexOffset;
        uint32_t m_IndexOffset;
        uint32_t m_IndexCount;
        uint32_t m_MaterialIndex;
        uint32_t m_UseViewModel;
        int _pad1;
    };

    struct alignas(16) GpuSkinnedMesh
    {
        glm::mat4 m_Transform;
        glm::mat4 m_NormalMatrix;
        glm::vec3 m_AABBMin;
        float _pad0;
        glm::vec3 m_AABBMax;
        int m_Selected;
        uint32_t m_VertexOffset;
        uint32_t m_IndexOffset;
        uint32_t m_IndexCount;
        uint32_t m_MaterialIndex;
        uint32_t m_BoneMatrixOffset;
        uint32_t m_BoneCount;
        uint32_t m_UseViewModel;
        int _pad1;
    };

    struct alignas(16) GpuCamera
    {
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
        glm::vec3 m_CameraPos;
        float _pad0;
    };

    struct alignas(16) GpuDirectionalLight
    {
        glm::vec3 m_Color;
        float m_Intensity;
        glm::vec3 m_Direction;
        float _pad0;
        glm::mat4 m_LightSpaceMatrix;
    };

    struct alignas(16) GpuPointLight
    {
        glm::vec3 m_Color;
        float m_Intensity;
        glm::mat4 m_Transform;
    };

    struct alignas(16) GpuSpotLight
    {
        glm::vec3 m_Color;
        float m_Intensity;
        glm::mat4 m_Transform;
        glm::vec2 m_ConeAngles;
        float m_Radius;
        float _pad0;
    };

    struct alignas(16) GpuDrawCommand
    {
        int m_Count;
        int m_InstanceCount;
        int m_FirstIndex;
        int m_BaseVertex;
        int m_BaseInstance;
        int _pad0[3];
    };

    struct alignas(16) GpuVoxelGrid
    {
        glm::vec3 m_GridMin;
        float _pad0;
        glm::vec3 m_GridMax;
        float _pad1;
        glm::vec3 m_CellSize;
        float _pad2;
        glm::ivec3 m_Resolution;
        int m_MipCount;
    };

    struct alignas(16) GpuVoxelConeTrace
    {
        float m_MaxDistance;
        float m_StepFactor;
        int m_NumCones;
        int m_NumSteps;

        float m_Aperture;
        float m_SpecularAperture;
        float m_IndirectStrength;
        float m_SpecularStrength;
    };
}
