// Shadow.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

in vec2 v_TexCoord;
flat in uint v_MeshIndex;
flat in uint v_MaterialIndex;

void main()
{
    if (v_MaterialIndex >= materials.length())
        discard;

    GpuMaterial mat = materials[v_MaterialIndex];

    if (mat.m_BaseColor_TexIndex < 0)
        return;

    uint64_t handle = textureHandles[mat.m_BaseColor_TexIndex];
    vec4 baseColor = texture(sampler2D(handle), v_TexCoord) * mat.m_BaseColorFactor;

    if (baseColor.a < 0.05)
        discard;
}
