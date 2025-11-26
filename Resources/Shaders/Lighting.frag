// Lighting.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_GColor;
uniform sampler2D u_GNormal;
uniform sampler2D u_GPosition;
uniform sampler2D u_GMaterial;
uniform sampler2D u_ShadowMap;
uniform sampler2D u_DepthBuffer;

#define SHADOW_SAMPLES 16
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec2 VogelDisk(int index, float angle)
{
    float r = sqrt(float(index) + 0.5) / sqrt(float(SHADOW_SAMPLES));
    float theta = float(index) * 2.4 + angle;
    return vec2(cos(theta), sin(theta)) * r;
}

float Noise(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float SoftShadow(vec3 worldPos, vec3 N, vec3 L, mat4 lightSpaceMatrix)
{
    vec4 lightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 proj = lightSpace.xyz / lightSpace.w;
    proj = proj * 0.5 + 0.5;
    
    if (proj.z > 1.0 || any(lessThan(proj.xy, vec2(0.0))) || any(greaterThan(proj.xy, vec2(1.0))))
        return 0.0;
    
    float NdotL = max(dot(N, normalize(-L)), 0.0);
    float bias = mix(0.0005, 0.002, 1.0 - NdotL);
    
    float angle = Noise(gl_FragCoord.xy) * 6.28318;
    
    float shadow = 0.0;
    float filterSize = 0.002;
    
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 offset = VogelDisk(i, angle) * filterSize;
        float depth = texture(u_ShadowMap, proj.xy + offset).r;
        shadow += (depth < proj.z - bias) ? 1.0 : 0.0;
    }
    
    shadow /= float(SHADOW_SAMPLES);
    
    vec2 fade = smoothstep(0.0, 0.05, proj.xy) * (1.0 - smoothstep(0.95, 1.0, proj.xy));
    shadow *= fade.x * fade.y;
    
    shadow *= 1.0 - smoothstep(0.95, 1.0, proj.z);
    
    return shadow;
}

vec3 CalculateDirectionalLight(GpuLight light, vec3 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 worldPos, bool useShadow)
{
    vec3 L = normalize(-light.m_Direction);
    vec3 H = normalize(V + L);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001, 0.0001);
    
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    
    float NdotL = max(dot(N, L), 0.0);
    
    float shadow = 0.0;
    if (useShadow) {
        shadow = SoftShadow(worldPos, N, light.m_Direction, light.m_LightSpaceMatrix);
    }
    
    return (kD * albedo / PI + specular) * light.m_Color * light.m_Intensity * NdotL * (1.0 - shadow);
}

vec3 CalculatePointLight(GpuLight light, vec3 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 worldPos)
{
    vec3 L = normalize(light.m_Position - worldPos);
    vec3 H = normalize(V + L);
    float distance = length(light.m_Position - worldPos);
    
    float attenuation = 1.0;
    if (light.m_Radius > 0.0) {
        attenuation = clamp(1.0 - (distance / light.m_Radius), 0.0, 1.0);
        attenuation = attenuation * attenuation;
    } else {
        attenuation = 1.0 / (distance * distance + 1.0);
    }
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001, 0.0001);
    
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    
    float NdotL = max(dot(N, L), 0.0);
    
    return (kD * albedo / PI + specular) * light.m_Color * light.m_Intensity * attenuation * NdotL;
}

vec3 CalculateSpotLight(GpuLight light, vec3 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 worldPos)
{
    vec3 L = normalize(light.m_Position - worldPos);
    vec3 H = normalize(V + L);
    float distance = length(light.m_Position - worldPos);
    
    float attenuation = 1.0;
    if (light.m_Radius > 0.0) {
        attenuation = clamp(1.0 - (distance / light.m_Radius), 0.0, 1.0);
        attenuation = attenuation * attenuation;
    } else {
        attenuation = 1.0 / (distance * distance + 1.0);
    }
    
    vec3 spotDir = normalize(-light.m_Direction);
    float theta = dot(L, spotDir);
    float epsilon = light.m_ConeAngles.x - light.m_ConeAngles.y;
    float intensity = clamp((theta - light.m_ConeAngles.y) / epsilon, 0.0, 1.0);
    intensity = intensity * intensity;
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001, 0.0001);
    
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    
    float NdotL = max(dot(N, L), 0.0);
    
    return (kD * albedo / PI + specular) * light.m_Color * light.m_Intensity * attenuation * intensity * NdotL;
}

void main()
{
    vec4 albedoTex = texture(u_GColor, TexCoord);
    vec3 normal = texture(u_GNormal, TexCoord).xyz;
    vec3 worldPos = texture(u_GPosition, TexCoord).xyz;
    vec4 material = texture(u_GMaterial, TexCoord);

    float depth = texture(u_DepthBuffer, TexCoord).r;
    
    if (depth >= 0.9999) {
        discard;
    }
    
    vec3 N = normalize(normal * 2.0 - 1.0);
    vec3 V = normalize(camera.m_Position - worldPos);
    
    float metallic = material.r;
    float roughness = material.g;
    float ao = material.b;
    float emissive = material.a;
    
    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < lights.length(); i++)
    {
        GpuLight light = lights[i];
        
        vec3 lightDir;
        if (light.m_Type == LIGHT_TYPE_DIRECTIONAL) {
            lightDir = normalize(-light.m_Direction);
        } else if (light.m_Type == LIGHT_TYPE_POINT) {
            lightDir = normalize(light.m_Position - worldPos);
        } else if (light.m_Type == LIGHT_TYPE_SPOT) {
            lightDir = normalize(light.m_Position - worldPos);
        }
        
        float NdotL = dot(N, lightDir);
        if (NdotL <= 0.0) {
            continue;
        }
        
        if (light.m_Type == LIGHT_TYPE_DIRECTIONAL)
        {
            bool useShadow = (i == 0);
            Lo += CalculateDirectionalLight(light, albedoTex.rgb, metallic, roughness, N, V, worldPos, useShadow);
        }
        else if (light.m_Type == LIGHT_TYPE_POINT)
        {
            Lo += CalculatePointLight(light, albedoTex.rgb, metallic, roughness, N, V, worldPos);
        }
        else if (light.m_Type == LIGHT_TYPE_SPOT)
        {
            Lo += CalculateSpotLight(light, albedoTex.rgb, metallic, roughness, N, V, worldPos);
        }
    }
    
    FragColor = vec4(Lo, albedoTex.a);
}