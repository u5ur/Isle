// Forward.frag

#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

layout(location = 0) in vec3 in_WorldPos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) flat in uint in_MaterialIndex;

layout(location = 0) out vec4 FragColor;

void main()
{
    GpuMaterial material = materials[in_MaterialIndex];

    vec4 baseColor = material.m_BaseColorFactor;
    if (material.m_BaseColor_TexIndex >= 0) {
        sampler2D texSampler = sampler2D(textureHandles[material.m_BaseColor_TexIndex]);
        baseColor *= texture(texSampler, in_TexCoord);
    }

    vec3 N = normalize(in_Normal);
    if (material.m_Normal_TexIndex >= 0) {
        sampler2D normalSampler = sampler2D(textureHandles[material.m_Normal_TexIndex]);
        vec3 tangentNormal = texture(normalSampler, in_TexCoord).xyz * 2.0 - 1.0;
        tangentNormal.xy *= material.m_NormalScale;

        vec3 T = normalize(in_Tangent);
        vec3 B = normalize(in_Bitangent);
        mat3 TBN = mat3(T, B, N);
        N = normalize(TBN * tangentNormal);
    }

    vec3 lightDir = normalize(vec3(-0.3, -0.9, -0.2));
    vec3 lightColor = vec3(1.0, 0.95, 0.85);
    float lightIntensity = 4.0;

    vec3 viewDir = normalize(camera.m_Position.xyz - in_WorldPos);

    float NdotL = max(dot(N, lightDir), 0.0);
    vec3 diffuse = NdotL * lightColor * baseColor.rgb * lightIntensity;

    vec3 halfDir = normalize(lightDir + viewDir);
    float specPower = 64.0;
    float specStrength = 0.25;
    float spec = pow(max(dot(N, halfDir), 0.0), specPower);
    vec3 specular = lightColor * spec * specStrength;

    vec3 ambient = vec3(0.08, 0.09, 0.10) * baseColor.rgb;

    vec3 emissive = material.m_EmissiveFactor;
    if (material.m_Emissive_TexIndex >= 0) {
        sampler2D emissiveSampler = sampler2D(textureHandles[material.m_Emissive_TexIndex]);
        emissive *= texture(emissiveSampler, in_TexCoord).rgb;
    }

    vec3 color = ambient + diffuse + specular + emissive;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, baseColor.a);
}
