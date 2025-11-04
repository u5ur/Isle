// Structs.glsl

struct GpuVertex
{
    vec3 m_Position;
    float _pad0;
    vec3 m_Normal;
    float _pad1;
    vec2 m_TexCoord;
    vec2 _pad2;
    vec3 m_Tangent;
    float _pad3;
    vec3 m_BitTangent;
    float _pad4;
    vec4 m_Color;
};

struct GpuSkinnedVertex
{
    vec3 m_Position;
    float _pad0;
    vec3 m_Normal;
    float _pad1;
    vec2 m_TexCoord;
    vec2 _pad2;
    vec3 m_Tangent;
    float _pad3;
    vec3 m_BitTangent;
    float _pad4;
    vec4 m_Color;
    ivec4 m_BoneIds;
    vec4 m_BoneWeights;
};

struct GpuMaterial
{
    int m_BaseColor_TexIndex;
    int m_Normal_TexIndex;
    int m_MetallicRoughness_TexIndex;
    int m_Occlusion_TexIndex;

    int m_Emissive_TexIndex;
    int _pad0[3];

    vec4 m_BaseColorFactor;
    vec3 m_EmissiveFactor;
    float m_IOR;
    float m_MetallicFactor;
    float m_RoughnessFactor;
    float m_NormalScale;
    float m_OcclusionStrength;
    int m_Transparent;
    float _pad2[2];
};

struct GpuStaticMesh
{
    mat4 m_Transform;
    mat4 m_NormalMatrix;
    vec3 m_AABBMin;
    float _pad0;
    vec3 m_AABBMax;
    int m_Selected;
    uint32_t m_VertexOffset;
    uint32_t m_IndexOffset;
    uint32_t m_IndexCount;
    uint32_t m_MaterialIndex;
    uint32_t m_UseViewModel;
    int _pad1;
};

struct GpuSkinnedMesh
{
    mat4 m_Transform;
    mat4 m_NormalMatrix;
    vec3 m_AABBMin;
    float _pad0;
    vec3 m_AABBMax;
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

struct GpuCamera
{
    mat4 m_ViewMatrix;
    mat4 m_ProjectionMatrix;
    vec3 m_Position;
    float _pad0;
};

struct GpuLight
{
    vec3 m_Color;
    float m_Intensity;
    vec3 m_Position;
    float m_Radius;
    vec3 m_Direction;
    int m_Type;
    vec2 m_ConeAngles;
    vec2 _pad0;
    mat4 m_LightSpaceMatrix;
    mat4 m_ShadowMatrices[6];
};

struct GpuDrawCommand
{
    int m_Count;
    int m_InstanceCount;
    int m_FirstIndex;
    int m_BaseVertex;
    int m_BaseInstance;
    int _pad0[3];
};

struct GpuVoxelGrid
{
    vec3 m_GridMin;
    float _pad0;
    vec3 m_GridMax;
    float _pad1;
    vec3 m_CellSize;
    float _pad2;
    ivec3 m_Resolution;
    int m_MipCount;
};