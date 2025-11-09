// Geometry.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

layout(location = 0) in vec3 In_WorldPos;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_TexCoord;
layout(location = 3) in vec3 In_Tangent;
layout(location = 4) in vec3 In_Bitangent;
layout(location = 5) flat in uint In_MaterialIndex;
layout(location = 6) flat in uint In_MeshIndex;

layout(location = 0) out vec4 Out_Color;
layout(location = 1) out vec4 Out_Normal;
layout(location = 2) out vec4 Out_Position;
layout(location = 3) out vec4 Out_Material;
layout(location = 4) out vec4 Out_Emissive;
layout(location = 5) out vec4 Out_Selection;

vec4 TrySampleTexture(int texIndex, vec2 uv)
{
    if (texIndex < 0) return vec4(1.0);
    uvec2 handleParts = unpackUint2x32(textureHandles[texIndex]);
    sampler2D tex = sampler2D(uint64_t(textureHandles[texIndex]));
    return texture(tex, uv);
}

void main()
{
    GpuMaterial mat = materials[In_MaterialIndex];

    vec4 baseColor = mat.m_BaseColorFactor;
    if (mat.m_BaseColor_TexIndex >= 0)
        baseColor *= TrySampleTexture(mat.m_BaseColor_TexIndex, In_TexCoord);

    if (baseColor.a <= 0.01f)
        discard;

    vec3 emissive = mat.m_EmissiveFactor;
    if (mat.m_Emissive_TexIndex >= 0)
        emissive *= TrySampleTexture(mat.m_Emissive_TexIndex, In_TexCoord).rgb;

    vec3 normal = normalize(In_Normal);
    if (mat.m_Normal_TexIndex >= 0)
    {
        vec3 mapN = TrySampleTexture(mat.m_Normal_TexIndex, In_TexCoord).xyz * 2.0 - 1.0;
        mapN.xy *= mat.m_NormalScale;
        mat3 TBN = mat3(normalize(In_Tangent), normalize(In_Bitangent), normalize(In_Normal));
        normal = normalize(TBN * mapN);
    }

    Out_Color = baseColor;
    Out_Normal = vec4(normal * 0.5 + 0.5, 1.0);
    Out_Position = vec4(In_WorldPos,  mat.m_IOR);
    Out_Material = vec4(mat.m_MetallicFactor, mat.m_RoughnessFactor, mat.m_IOR, 1.0);
    Out_Emissive = vec4(emissive, 1.0);

    GpuStaticMesh mesh = meshes[In_MeshIndex];
    Out_Selection = mesh.m_Selected == 1 ? vec4(1.0, 0.0, 0.0, 1.0) : vec4(0.0);
}
