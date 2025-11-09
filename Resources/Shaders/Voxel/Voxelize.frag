// Voxelize.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_NV_shader_atomic_fp16_vector : require
#include "../Common/Common.glsl"

in FragmentData
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    flat uint MaterialIndex;
    flat vec3 TriNormal;
} fs_in;

layout(binding = 0, rgba16f) coherent uniform image3D u_AtomicRadiance;
layout(binding = 1, rgba16f) coherent uniform image3D u_AtomicNormal;
layout(binding = 2, r32ui) coherent uniform uimage3D u_AtomicCount;

uniform ivec3 u_Resolution;
uniform ivec3 u_GridMin;
uniform ivec3 u_GridMax;
uniform vec3 u_CellSize;
uniform sampler2D u_ShadowMap;

void main()
{
    vec3 gridExtent = vec3(u_GridMax - u_GridMin);
    vec3 uvw = (fs_in.FragPos - vec3(u_GridMin)) / gridExtent;
    ivec3 voxelCoord = ivec3(uvw * vec3(u_Resolution));

    if (any(lessThan(voxelCoord, ivec3(0))) ||
        any(greaterThanEqual(voxelCoord, u_Resolution)))
        discard;

    GpuMaterial material = materials[fs_in.MaterialIndex];
    vec3 albedo = material.m_BaseColorFactor.rgb;
    vec3 emissive = material.m_EmissiveFactor;

    if (material.m_BaseColor_TexIndex >= 0)
        albedo *= texture(sampler2D(textureHandles[material.m_BaseColor_TexIndex]), fs_in.TexCoord).rgb;

    if (material.m_Emissive_TexIndex >= 0)
        emissive *= texture(sampler2D(textureHandles[material.m_Emissive_TexIndex]), fs_in.TexCoord).rgb;

    vec3 N = normalize(fs_in.TriNormal);
    vec3 baseLighting = vec3(0.02);

    for (int i = 0; i < lights.length() && i < 4; i++)
    {
        GpuLight light = lights[i];
        vec3 L = -light.m_Direction;
        float NdotL = max(dot(N, L), 0.0);
        baseLighting += light.m_Color * light.m_Intensity * NdotL;
    }

    vec3 radiance = emissive + albedo * baseLighting;
    f16vec4 radianceToAdd = f16vec4(f16vec3(radiance), float16_t(0.0));
    imageAtomicAdd(u_AtomicRadiance, voxelCoord, radianceToAdd);

    vec3 normalDir = normalize(fs_in.TriNormal) * 0.5 + 0.5;
    f16vec4 normalToAdd = f16vec4(f16vec3(normalDir), float16_t(0.0));
    imageAtomicAdd(u_AtomicNormal, voxelCoord, normalToAdd);

    imageAtomicAdd(u_AtomicCount, voxelCoord, 1u);
    memoryBarrierImage();
}