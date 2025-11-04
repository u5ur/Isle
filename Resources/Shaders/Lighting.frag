// Lighting.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

layout(location = 0) in vec2 Out_TexCoord;
layout(location = 0) out vec4 Out_Color;

uniform sampler2D u_GColor;
uniform sampler2D u_GNormal;
uniform sampler2D u_GPosition;
uniform sampler2D u_GMaterial;

void main()
{
    vec4 albedo = texture(u_GColor, Out_TexCoord);
    vec3 normal = texture(u_GNormal, Out_TexCoord).xyz * 2.0 - 1.0;
    vec3 worldPos = texture(u_GPosition, Out_TexCoord).xyz;
    vec4 materialData = texture(u_GMaterial, Out_TexCoord);

    if (albedo.a < 0.01)
    {
        Out_Color = vec4(0.0);
        return;
    }

    vec3 lightDir = normalize(vec3(-0.3, -0.9, -0.2));
    vec3 lightColor = vec3(1.0, 0.95, 0.85);
    float lightIntensity = 4.0;

    vec3 viewDir = normalize(camera.m_Position.xyz - worldPos);
    vec3 N = normalize(normal);

    float NdotL = max(dot(N, lightDir), 0.0);
    vec3 diffuse = NdotL * lightColor * albedo.rgb * lightIntensity;

    vec3 halfDir = normalize(lightDir + viewDir);
    float specPower = 64.0;
    float specStrength = 0.25;
    float spec = pow(max(dot(N, halfDir), 0.0), specPower);
    vec3 specular = lightColor * spec * specStrength;

    vec3 ambient = vec3(0.08, 0.09, 0.10) * albedo.rgb;
    vec3 color = ambient + diffuse + specular;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    Out_Color = vec4(color, 1.0);
}
