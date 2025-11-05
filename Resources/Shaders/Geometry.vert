// Geometry.vert
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

layout(location = 0) out vec3 Out_WorldPos;
layout(location = 1) out vec3 Out_Normal;
layout(location = 2) out vec2 Out_TexCoord;
layout(location = 3) out vec3 Out_Tangent;
layout(location = 4) out vec3 Out_Bitangent;
layout(location = 5) flat out uint Out_MaterialIndex;
layout(location = 6) flat out uint Out_MeshIndex;

void main()
{
    uint meshIndex = gl_BaseInstance;
    GpuStaticMesh mesh = meshes[meshIndex];

    uint vertexIndex = mesh.m_VertexOffset + gl_VertexID;
    GpuVertex v = vertices[vertexIndex];

    mat3 normalMat = mat3(mesh.m_NormalMatrix);
    vec4 worldPos = mesh.m_Transform * vec4(v.m_Position, 1.0);

    Out_WorldPos = worldPos.xyz;
    Out_Normal = normalize(normalMat * v.m_Normal);
    Out_Tangent = normalize(normalMat * v.m_Tangent);
    Out_Bitangent = normalize(normalMat * v.m_BitTangent);
    Out_TexCoord = v.m_TexCoord;
    Out_MaterialIndex = mesh.m_MaterialIndex;
    Out_MeshIndex = meshIndex;

    gl_Position = camera.m_ProjectionMatrix * camera.m_ViewMatrix * worldPos;
}
