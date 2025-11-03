// Forward.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

#include "Common\Common.glsl"

layout(location = 0) in vec3 in_WorldPos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) flat in uint in_MaterialIndex;

layout(location = 0) out vec4 FragColor;

void main()
{
    GPU_Material material = materials[in_MaterialIndex];
    
    vec4 baseColor = material.m_BaseColorFactor;
    if (material.m_BaseColor_TexIndex >= 0) {
        sampler2D texSampler = sampler2D(textureHandles[material.m_BaseColor_TexIndex]);
        vec4 texColor = texture(texSampler, in_TexCoord);
        baseColor *= texColor;
    }
    
    vec3 N = normalize(in_Normal);
    if (material.m_Normal_TexIndex >= 0) 
    {
        sampler2D normalSampler = sampler2D(textureHandles[material.m_Normal_TexIndex]);
        vec3 tangentNormal = texture(normalSampler, in_TexCoord).xyz * 2.0 - 1.0;
        tangentNormal.xy *= material.m_NormalScale;
        
        vec3 T = normalize(in_Tangent);
        vec3 B = normalize(in_Bitangent);
        mat3 TBN = mat3(T, B, N);
        N = normalize(TBN * tangentNormal);
    }
    
    vec3 L = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(N, L), 0.0);
    
    vec3 ambient = 0.2 * baseColor.rgb;
    vec3 diffuse = diff * baseColor.rgb;
    
    vec3 emissive = material.m_EmissiveFactor;
    if (material.m_Emissive_TexIndex >= 0) {
        sampler2D emissiveSampler = sampler2D(textureHandles[material.m_Emissive_TexIndex]);
        vec3 emissiveTex = texture(emissiveSampler, in_TexCoord).rgb;
        emissive *= emissiveTex;
    }
    
    vec3 color = ambient + diffuse + emissive;  
    FragColor = vec4(color, 1.0);
}