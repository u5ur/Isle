// Structs.glsl

struct GPU_Vertex
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

struct GPU_Material
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

struct GPU_Mesh
{
    mat4 m_Transform;
    mat4 m_NormalMatrix;
    vec3 m_AABBMin;
    float _pad0;
    vec3 m_AABBMax;
    int m_Selected;
    uint m_VertexOffset;
    uint m_IndexOffset;
    uint m_IndexCount;
    uint m_MaterialIndex;
    uint m_UseViewModel;
    int _pad1;
};

struct GPU_Camera
{
    mat4 m_ViewMatrix;
    mat4 m_ProjectionMatrix;
    vec3 m_CameraPos; 
    float _pad0;
};

struct GPU_DrawCommand
{
    int m_Count;
    int m_InstanceCount;
    int m_FirstIndex;
    int m_BaseVertex;
    int m_BaseInstance;
    int _pad0[3];
};

