// Composite.frag
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
uniform sampler2D u_DirectLighting;
uniform sampler2D u_Selection;
uniform sampler2D u_DepthBuffer;

uniform sampler3D u_VoxelRadiance;
uniform sampler3D u_VoxelNormal;

uniform ivec3 u_Resolution;
uniform ivec3 u_GridMin;
uniform ivec3 u_GridMax;
uniform int u_MipCount;
uniform vec3 u_CellSize;

uniform bool u_EnableGI = true;
uniform bool u_EnableReflections = true;
uniform bool u_EnableTonemapping = true;
uniform bool u_EnableAO = true;
uniform float u_AOIntensity = 1.0;
uniform float u_AORadius = 2.0;

uniform float u_IndirectStrength = 1.0;
uniform float u_SpecularStrength = 1.0;
uniform float u_MaxDistance = 50.0;

uniform int u_SSR_MaxSteps = 128;
uniform int u_SSR_BinarySteps = 8;
uniform float u_SSR_StepSize = 0.5;
uniform float u_SSR_MaxRayDistance = 100.0;
uniform float u_SSR_DepthThickness = 0.5;
uniform float u_SSR_EdgeFadeStart = 0.8;
uniform float u_SSR_EdgeFadeEnd = 0.95;

uniform vec3 u_OutlineColor = vec3(1.0, 0.6, 0.0);

const float CONE_TRACE_MIN_DIAMETER = 0.5;
const float AO_BIAS = 0.1;
const float SPECULAR_EARLY_OUT_THRESHOLD = 0.95;
const float DIFFUSE_EARLY_OUT_THRESHOLD = 0.90;


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

vec3 SampleConeDirection(vec3 normal, vec3 tangent, vec3 bitangent, int index, int total)
{
    float goldenRatio = 2.39996322972865332;
    float theta = float(index) * goldenRatio;
    float phi = acos(1.0 - (float(index) + 0.5) / float(total));
    
    float sinPhi = sin(phi);
    vec3 dir = sinPhi * cos(theta) * tangent +
               sinPhi * sin(theta) * bitangent +
               cos(phi) * normal;
    
    return normalize(dir);
}

vec3 IORToF0(float ior, vec3 albedo, float metallic)
{
    float baseF0 = pow((ior - 1.0) / (ior + 1.0), 2.0);
    return mix(vec3(baseF0), albedo, metallic);
}

vec3 TraceSpecularCone(vec3 origin, vec3 direction, float roughness, float maxDist)
{
    vec3 radiance = vec3(0.0);
    float alpha = 0.0;
    
    float coneAperture = max(0.0001, roughness * roughness * 0.5);
    float minVoxelDiameter = u_CellSize.x * CONE_TRACE_MIN_DIAMETER;    
    float dist = minVoxelDiameter * 2.0;
    
    const int MAX_STEPS = 128;
    int step = 0;
    
    while (dist < maxDist && alpha < 0.99 && step < MAX_STEPS)
    {
        float coneDiameter = max(minVoxelDiameter, 2.0 * tan(coneAperture) * dist);  
        float lod = log2(coneDiameter / u_CellSize.x);
        lod = clamp(lod, 0.0, float(u_MipCount - 1));
        
        vec3 samplePos = origin + direction * dist;
        vec4 voxelSample = SampleVoxelRadianceSafe(samplePos, lod);
        
        if (voxelSample.a > 0.001)
        {
            float weight = voxelSample.a * (1.0 - alpha);
            radiance += voxelSample.rgb * weight;
            alpha += weight;
        }
        
        dist += max(coneDiameter * 0.5, minVoxelDiameter);
        step++;
    }
    
    return radiance;
}

vec3 ComputeIndirectDiffuse(vec3 position, vec3 normal, float roughness)
{
    vec3 tangent, bitangent;
    BuildTangentBasis(normal, tangent, bitangent);
    
    vec3 gi = vec3(0.0);
    float totalWeight = 0.0;
    float maxDist = u_MaxDistance * 0.4;
    float minVoxelDiameter = u_CellSize.x * CONE_TRACE_MIN_DIAMETER;
    float coneRatio = mix(0.35, 0.75, roughness);
    
    // Main cone
    {
        vec3 radiance = vec3(0.0);
        float coneWeight = 0.0;
        float occlusion = 0.0;
        float dist = minVoxelDiameter * 3.0;
        
        for (int step = 0; step < 64; step++)
        {
            if (dist >= maxDist) break;
            
            float coneDiameter = max(2.0 * coneRatio * dist, minVoxelDiameter);
            float lod = clamp(log2(coneDiameter / u_CellSize.x), 0.0, float(u_MipCount - 1));
            
            vec4 sample2 = SampleVoxelRadianceSafe(position + normal * dist, lod);
            
            if (sample2.a > 0.01)
            {
                float falloff = 1.0 / (1.0 + 0.05 * dist);
                float weight = sample2.a * (1.0 - min(occlusion, 0.98)) * falloff;
                radiance += sample2.rgb * weight;
                coneWeight += weight;
                occlusion += sample2.a * (1.0 - min(occlusion, 0.98)) * 0.35;
            }
            
            dist += coneDiameter * 1.2;
        }
        
        if (coneWeight > 0.0001)
            radiance /= coneWeight;
        
        gi += radiance * 0.3;
        totalWeight += 0.3;
    }
    
    // Side cones
    for (int i = 0; i < 4; i++)
    {
        float angle = float(i) * 1.5708;
        vec3 offset = cos(angle) * tangent + sin(angle) * bitangent;
        vec3 coneDir = normalize(normal + offset * 0.5);
        
        vec3 radiance = vec3(0.0);
        float coneWeight = 0.0;
        float occlusion = 0.0;
        float dist = minVoxelDiameter * 3.0;
        
        for (int step = 0; step < 64; step++)
        {
            if (dist >= maxDist) break;
            
            float coneDiameter = max(2.0 * coneRatio * dist, minVoxelDiameter);
            float lod = clamp(log2(coneDiameter / u_CellSize.x) + 0.5, 0.0, float(u_MipCount - 1));
            
            vec4 sample2 = SampleVoxelRadianceSafe(position + coneDir * dist, lod);
            
            if (sample2.a > 0.01)
            {
                float falloff = 1.0 / (1.0 + 0.05 * dist);
                float weight = sample2.a * (1.0 - min(occlusion, 0.98)) * falloff;
                radiance += sample2.rgb * weight;
                coneWeight += weight;
                occlusion += sample2.a * (1.0 - min(occlusion, 0.98)) * 0.3;
            }
            
            dist += coneDiameter * 1.3;
        }
        
        if (coneWeight > 0.0001)
            radiance /= coneWeight;
        
        gi += radiance * 0.175;
        totalWeight += 0.175;
    }
    
    return gi * u_IndirectStrength;
}

vec3 ComputeSpecularReflection(vec3 position, vec3 normal, vec3 viewDir, float roughness)
{
    if (roughness > 0.95)
        return vec3(0.0);
    
    vec3 reflectDir = reflect(-viewDir, normal);
    float maxDist = u_MaxDistance * mix(1.0, 0.5, roughness * roughness);
    
    return TraceSpecularCone(position, reflectDir, roughness, maxDist) * u_SpecularStrength;
}

float ComputeVoxelAO(vec3 position, vec3 normal)
{
    vec3 tangent, bitangent;
    BuildTangentBasis(normal, tangent, bitangent);
    
    float occlusion = 0.0;
    const int numRays = 8;
    float radius = u_AORadius * u_CellSize.x;
    
    for (int i = 0; i < numRays; i++)
    {
        vec3 rayDir = SampleConeDirection(normal, tangent, bitangent, i, numRays);
        
        float rayOcclusion = 0.0;
        const int numSteps = 4;
        float stepSize = radius / float(numSteps);
        
        for (int step = 1; step <= numSteps; step++)
        {
            float dist = stepSize * float(step);
            vec3 samplePos = position + rayDir * dist;
            
            vec4 voxelSample = SampleVoxelRadianceSafe(samplePos, 0.0);
            
            float falloff = 1.0 - (dist / radius);
            falloff = falloff * falloff;
            
            rayOcclusion += voxelSample.a * falloff;
        }
        
        occlusion += rayOcclusion / float(numSteps);
    }
    
    occlusion /= float(numRays);
    occlusion = 1.0 - clamp(occlusion, 0.0, 1.0);
    occlusion = pow(occlusion, u_AOIntensity);
    
    return occlusion;
}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 LinearToSRGB(vec3 linear)
{
    bvec3 cutoff = lessThan(linear, vec3(0.0031308));
    vec3 higher = vec3(1.055) * pow(linear, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linear * vec3(12.92);
    return mix(higher, lower, cutoff);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    vec3 maxFresnel = max(vec3(1.0 - roughness), F0);
    return F0 + (maxFresnel - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 HandleOutline(vec2 uv, vec3 baseColor)
{
    float center = texture(u_Selection, uv).r;
    if (center < 0.5)
        return baseColor;

    vec2 texel = 1.0 / vec2(textureSize(u_Selection, 0));
    float edge = 0.0;

    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            vec2 offset = vec2(x, y) * texel;
            if (texture(u_Selection, uv + offset).r < 0.5)
                edge += 1.0;
        }
    }

    if (edge > 0.0)
    {
        float intensity = clamp(edge / 8.0, 0.0, 1.0);
        return mix(baseColor, u_OutlineColor, intensity);
    }

    return baseColor;
}

void main()
{
    vec3 worldPos = texture(u_GPosition, TexCoord).rgb;   
    if (dot(worldPos, worldPos) < 0.001)
    {
        FragColor = texture(u_DirectLighting, TexCoord);
        return;
    }
    
    vec3 directLighting = texture(u_DirectLighting, TexCoord).rgb;
    vec3 albedo = texture(u_GColor, TexCoord).rgb;
    vec3 normal = normalize(texture(u_GNormal, TexCoord).rgb * 2.0 - 1.0);
    vec4 material = texture(u_GMaterial, TexCoord);
    
    float metallic = material.r;
    float ior = texture(u_GPosition, TexCoord).a;
    float roughness = max(material.g, 0.04);
    float ao = material.b;
    
    vec3 viewDir = normalize(camera.m_Position - worldPos);
    vec3 finalColor = directLighting;
            vec3 indirectDiffuse = ComputeIndirectDiffuse(worldPos, normal, roughness);

    if (u_EnableGI)
    {
        vec3 kD = vec3(1.0 - metallic);
        finalColor += indirectDiffuse * albedo * kD;
    }
    
    if (u_EnableReflections)
    {
        vec3 F0 = IORToF0(ior, albedo, metallic); 
        vec3 specularReflection = ComputeSpecularReflection(worldPos, normal, viewDir, roughness); 
        float NdotV = max(dot(normal, viewDir), 0.0);
        vec3 fresnel = FresnelSchlickRoughness(NdotV, F0, roughness);   
        finalColor += specularReflection * fresnel;
    }
    
    if (u_EnableAO)
    {
        float voxelAO = ComputeVoxelAO(worldPos, normal);
        float combinedAO = min(voxelAO, ao);
        finalColor *= mix(1.0, combinedAO, u_AOIntensity);
    }
    
    if (u_EnableTonemapping)
    {
        finalColor = ACESFilm(finalColor);
        finalColor = LinearToSRGB(finalColor);
    }

    float selected = texture(u_Selection, TexCoord).r;
    if (selected > 0.5)
    {
        finalColor = HandleOutline(TexCoord, finalColor);
    }
    
    FragColor = vec4(finalColor, 1.0);
}