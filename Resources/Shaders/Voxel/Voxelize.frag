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
layout(binding = 1, r32ui) coherent uniform uimage3D u_AtomicCount;

uniform ivec3 u_Resolution;
uniform ivec3 u_GridMin;
uniform ivec3 u_GridMax;
uniform vec3 u_CellSize;
uniform sampler2D u_ShadowMap;

float SampleShadow(vec3 worldPos, vec3 N, vec3 lightDir)
{
    if (lights.length() == 0)
        return 1.0;

    GpuLight light = lights[0];
    vec4 lightSpacePos = light.m_LightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 proj = lightSpacePos.xyz / lightSpacePos.w;
    proj = proj * 0.5 + 0.5;

    if (proj.z > 1.0 || any(lessThan(proj.xy, vec2(0.0))) || any(greaterThan(proj.xy, vec2(1.0))))
        return 1.0;

    float NdotL = max(dot(N, normalize(-lightDir)), 0.0);
    float bias = mix(0.0005, 0.002, 1.0 - NdotL);

    float shadowDepth = texture(u_ShadowMap, proj.xy).r;
    return (proj.z - bias > shadowDepth) ? 0.0 : 1.0;
}

vec3 hash3(vec3 p)
{
    p = vec3(dot(p, vec3(127.1, 311.7, 74.7)),
             dot(p, vec3(269.5, 183.3, 246.1)),
             dot(p, vec3(113.5, 271.9, 124.6)));
    return fract(sin(p) * 43758.5453123);
}

void main()
{
    // Convert to your old coordinate calculation
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

    // Directional lights - exactly like your old code
    for (int i = 0; i < lights.length() && i < 4; i++) {
        GpuLight light = lights[i];
        vec3 L = -light.m_Direction;
        float NdotL = max(dot(N, L), 0.0);
        float shadow = SampleShadow(fs_in.FragPos, N, L);
        baseLighting += light.m_Color * light.m_Intensity * NdotL * shadow;
    }

    // Point lights - exactly like your old code  
    // (You'll need to add point light support to your current system)
    /*
    for (int i = 0; i < pointLights.length() && i < 8; i++) {
        vec3 lightPos = pointLights[i].m_Transform[3].xyz;
        vec3 L = lightPos - fs_in.FragPos;
        float distance = length(L);
        L = normalize(L);
        float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
        float NdotL = max(dot(N, L), 0.0);
        baseLighting += pointLights[i].m_Color * pointLights[i].m_Intensity * NdotL * attenuation;
    }
    */

    vec3 radiance = emissive + albedo * baseLighting;

    // Exact dithering from your old code
    vec3 dither3D = hash3(vec3(voxelCoord) + fs_in.FragPos * 0.01) - 0.5;
    vec3 dithered = clamp(radiance + dither3D * (0.5 / 65535.0), 0.0, 65504.0);

    // Exact atomic operations from your old code
    f16vec4 radianceToAdd = f16vec4(f16vec3(dithered), float16_t(0.0));
    imageAtomicAdd(u_AtomicRadiance, voxelCoord, radianceToAdd);
    memoryBarrierImage();

    imageAtomicAdd(u_AtomicCount, voxelCoord, 1u);
    memoryBarrierImage();
}