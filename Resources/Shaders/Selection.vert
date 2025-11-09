// Selection.vert
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

layout(location = 0) flat out uint Out_MeshIndex;

void main()
{
    uint meshIndex = gl_BaseInstance;
    GpuStaticMesh mesh = meshes[meshIndex];
    GpuVertex vertex = vertices[gl_VertexID];
    vec4 worldPos = mesh.m_Transform * vec4(vertex.m_Position, 1.0);

    Out_MeshIndex = meshIndex;
    gl_Position = camera.m_ProjectionMatrix * camera.m_ViewMatrix * worldPos;
}
