// Structs.glsl

struct GpuVertex
{
    vec3 m_Position;
    uint m_NormalTangent;
    vec2 m_TexCoord;
    uint m_Color;
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

struct UnpackedVertex
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 texCoord;
    vec4 color;
};

float unpack10BitSNorm(int p)
{
    int value = (p >= 512) ? (p - 1024) : p;
    return clamp(float(value) / 511.0, -1.0, 1.0);
}

vec4 unpackNormalTangent(uint p)
{
    int nx = int(p & 0x3FFu);
    int ny = int((p >> 10u) & 0x3FFu);
    int nz = int((p >> 20u) & 0x3FFu);
    uint handedness = (p >> 30u) & 0x3u;

    vec3 normal = vec3(
        unpack10BitSNorm(nx),
        unpack10BitSNorm(ny),
        unpack10BitSNorm(nz)
    );
    normal = normalize(normal);

    float tangentW = (handedness > 0u) ? 1.0 : -1.0;
    return vec4(normal, tangentW);
}

vec4 unpackColor(uint p)
{
    return vec4(
        float(p & 0xFFu) / 255.0,
        float((p >> 8u) & 0xFFu) / 255.0,
        float((p >> 16u) & 0xFFu) / 255.0,
        float((p >> 24u) & 0xFFu) / 255.0
    );
}

UnpackedVertex unpackVertex(GpuVertex vertex)
{
    UnpackedVertex unpacked;
    unpacked.position = vertex.m_Position;
    unpacked.texCoord = vertex.m_TexCoord;
    unpacked.color = unpackColor(vertex.m_Color);
    
    vec4 normalTangentW = unpackNormalTangent(vertex.m_NormalTangent);
    unpacked.normal = normalTangentW.xyz;
    float tangentW = normalTangentW.w;
    
    vec3 helper = (abs(unpacked.normal.x) > 0.999) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    unpacked.tangent = normalize(cross(unpacked.normal, helper));
    unpacked.bitangent = cross(unpacked.normal, unpacked.tangent) * tangentW;
    
    return unpacked;
}