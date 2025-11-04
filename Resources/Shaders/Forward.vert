#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common\Common.glsl"

layout(location = 0) out vec3 out_WorldPos;
layout(location = 1) out vec3 out_Normal;
layout(location = 2) out vec2 out_TexCoord;
layout(location = 3) out vec3 out_Tangent;
layout(location = 4) out vec3 out_Bitangent;
layout(location = 5) flat out uint out_MaterialIndex;

void main()
{
    GPU_Mesh mesh = meshes[gl_BaseInstance];
    
    uint indexValue = indices[mesh.m_IndexOffset + gl_VertexID];
    uint vertexIndex = mesh.m_VertexOffset + gl_VertexID;
    GPU_Vertex v = vertices[vertexIndex];
    
    out_WorldPos = vec3(mesh.m_Transform * vec4(v.m_Position, 1.0));
    out_Normal = normalize(mat3(mesh.m_NormalMatrix) * v.m_Normal);
    out_Tangent = normalize(mat3(mesh.m_Transform) * v.m_Tangent);
    out_Bitangent = normalize(mat3(mesh.m_Transform) * v.m_BitTangent);
    out_TexCoord = v.m_TexCoord;
    out_MaterialIndex = mesh.m_MaterialIndex;
    
    vec4 worldPos = mesh.m_Transform * vec4(v.m_Position, 1.0);
    gl_Position = camera.m_ProjectionMatrix * camera.m_ViewMatrix * worldPos;
}