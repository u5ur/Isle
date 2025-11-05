// Voxelize.vert
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "../Common/Common.glsl"

out VertexData
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    flat uint MaterialIndex;
} vs_out;

uniform ivec3 u_Resolution;
uniform ivec3 u_GridMin;
uniform ivec3 u_GridMax;
uniform vec3 u_CellSize;

void main()
{
    uint meshIndex = gl_BaseInstance;
    GpuStaticMesh mesh = meshes[meshIndex];
    uint vertexIndex = mesh.m_VertexOffset + gl_VertexID;
    GpuVertex vertex = vertices[vertexIndex];
    
    vec4 worldPos = mesh.m_Transform * vec4(vertex.m_Position, 1.0);
    mat3 normalMat = mat3(mesh.m_NormalMatrix);
    
    vec3 voxelPos = (worldPos.xyz - u_GridMin) / (u_GridMax - u_GridMin);
    
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = normalize(normalMat * vertex.m_Normal);
    vs_out.TexCoord = vertex.m_TexCoord;
    vs_out.MaterialIndex = mesh.m_MaterialIndex;
    
    gl_Position = vec4(voxelPos * 2.0 - 1.0, 1.0);
}