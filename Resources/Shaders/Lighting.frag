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

uniform sampler3D u_VoxelRadiance;
uniform sampler3D u_VoxelNormal;

uniform ivec3 u_Resolution;
uniform ivec3 u_GridMin;
uniform ivec3 u_GridMax;
uniform vec3 u_CellSize;

uniform float u_GIStrength = 1.0;
uniform float u_GIMaxDistance = 20.0;

#define CONE_TRACE_MIN_DIAMETER 0.5

vec3 WorldToVoxelUVW(vec3 worldPos)
{
    return (worldPos - u_GridMin) / (u_GridMax - u_GridMin);
}

vec4 SampleVoxelRadianceSafe(vec3 worldPos, float lod)
{
    vec3 uvw = WorldToVoxelUVW(worldPos);
    if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
        return vec4(0.0);
    return textureLod(u_VoxelRadiance, uvw, lod);
}

void BuildTangentBasis(vec3 normal, out vec3 tangent, out vec3 bitangent)
{
    vec3 up = abs(normal.y) < 0.9 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    tangent = normalize(cross(up, normal));
    bitangent = cross(normal, tangent);
}

vec3 TraceDiffuseCone(vec3 origin, vec3 direction, float roughness, float maxDist)
{
    vec3 radiance = vec3(0.0);
    float totalWeight = 0.0;
    float occlusion = 0.0;
    
    float coneRatio = 0.5;
    float minVoxelDiameter = u_CellSize.x * CONE_TRACE_MIN_DIAMETER;
    float dist = minVoxelDiameter * 3.0;
    
    const int MAX_STEPS = 32;
    int step = 0;
    
    while (dist < maxDist && occlusion < 0.95 && step < MAX_STEPS)
    {
        float coneDiameter = max(2.0 * coneRatio * dist, minVoxelDiameter);
        float lod = log2(coneDiameter / u_CellSize.x);
        lod = clamp(lod, 0.0, 5.0);
        
        vec3 samplePos = origin + direction * dist;
        vec4 voxelSample = SampleVoxelRadianceSafe(samplePos, lod);
        
        if (voxelSample.a > 0.01)
        {
            float falloff = 1.0 / (1.0 + 0.05 * dist);
            float sampleWeight = voxelSample.a * (1.0 - occlusion) * falloff;
            radiance += voxelSample.rgb * sampleWeight;
            totalWeight += sampleWeight;
            occlusion += voxelSample.a * (1.0 - occlusion) * 0.3;
        }
        
        dist += coneDiameter * 1.2;
        step++;
    }
    
    if (totalWeight > 0.0)
        radiance /= totalWeight;
    
    return radiance;
}

vec3 ComputeIndirectDiffuse(vec3 position, vec3 normal, float roughness)
{
    vec3 tangent, bitangent;
    BuildTangentBasis(normal, tangent, bitangent);
    
    vec3 gi = vec3(0.0);
    float totalWeight = 0.0;
    float maxDist = u_GIMaxDistance;
    
    {
        vec3 radiance = TraceDiffuseCone(position + normal * 0.1, normal, roughness, maxDist);
        gi += radiance * 0.3;
        totalWeight += 0.3;
    }
    
    for (int i = 0; i < 4; i++)
    {
        float angle = float(i) * 1.5708;
        vec3 offset = cos(angle) * tangent + sin(angle) * bitangent;
        vec3 coneDir = normalize(normal + offset * 0.5);
        
        vec3 radiance = TraceDiffuseCone(position + coneDir * 0.1, coneDir, roughness, maxDist);
        gi += radiance * 0.175;
        totalWeight += 0.175;
    }
    
    return gi * u_GIStrength;
}

vec2 VogelDisk(int i, float angle)
{
    float r = sqrt(float(i) + 0.5) / sqrt(float(16));
    float theta = float(i) * 2.4 + angle;
    return vec2(cos(theta), sin(theta)) * r;
}

float Noise(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float CalculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir)
{
    if (lights.length() == 0)
        return 0.0;
    
    GpuLight light = lights[0];
    
    vec4 lightSpacePos = light.m_LightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0 || any(lessThan(projCoords.xy, vec2(0.0))) || any(greaterThan(projCoords.xy, vec2(1.0))))
        return 0.0;
    
    float NdotL = max(dot(normal, -lightDir), 0.0);
    float bias = mix(0.005, 0.0005, NdotL);
    
    float shadow = 0.0;
    float angle = Noise(gl_FragCoord.xy) * 6.28318530718;
    float filterRadius = 0.002;
    
    for (int i = 0; i < 16; i++)
    {
        vec2 offset = VogelDisk(i, angle) * filterRadius;
        float shadowDepth = texture(u_ShadowMap, projCoords.xy + offset).r;
        shadow += (shadowDepth < projCoords.z - bias) ? 1.0 : 0.0;
    }
    shadow /= 16.0;
    
    vec2 fadeFactor = smoothstep(0.0, 0.05, projCoords.xy) * (1.0 - smoothstep(0.95, 1.0, projCoords.xy));
    shadow *= fadeFactor.x * fadeFactor.y;
    shadow *= 1.0 - smoothstep(0.95, 1.0, projCoords.z);
    
    return shadow;
}

vec3 CalculateDirectionalLight(GpuLight light, vec3 albedo, float metallic, float roughness, vec3 normal, vec3 viewDir, vec3 worldPos)
{
    vec3 lightDir = normalize(-light.m_Direction);
    vec3 halfDir = normalize(viewDir + lightDir);
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    float NDF = DistributionGGX(normal, halfDir, roughness);
    float G = GeometrySmith(normal, viewDir, -lightDir, roughness);
    vec3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, -lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    
    float NdotL = max(dot(normal, -lightDir), 0.0);
    float shadowFactor = CalculateShadow(worldPos, normal, lightDir);
    
    vec3 radiance = light.m_Color * light.m_Intensity;
    return (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadowFactor);
}

void main()
{
    vec4 albedo = texture(u_GColor, TexCoord);
    vec3 normal = normalize(texture(u_GNormal, TexCoord).xyz * 2.0 - 1.0);
    vec3 worldPos = texture(u_GPosition, TexCoord).xyz;
    vec4 materialData = texture(u_GMaterial, TexCoord);

    float metallic = materialData.r;
    float roughness = materialData.g;

    vec3 viewDir = normalize(camera.m_Position - worldPos);

    vec3 Lo = albedo.rgb * 0.03;
    if (lights.length() > 0)
        Lo += CalculateDirectionalLight(lights[0], albedo.rgb, metallic, roughness, normal, viewDir, worldPos);

    vec3 indirectDiffuse = ComputeIndirectDiffuse(worldPos, normal, roughness);
    vec3 kD = vec3(1.0 - metallic);
    Lo += indirectDiffuse * albedo.rgb * kD;

//   vec3 uvw = WorldToVoxelUVW(worldPos);
//        if (all(greaterThanEqual(uvw, vec3(0.0))) && all(lessThanEqual(uvw, vec3(1.0))))
//            Lo = textureLod(u_VoxelRadiance, uvw, 0.0).rgb;

    FragColor = vec4(Lo, albedo.a);
}