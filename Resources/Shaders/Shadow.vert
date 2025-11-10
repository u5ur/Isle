#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

#include "Common/Common.glsl"

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) flat out uint v_MeshIndex;
layout(location = 2) flat out uint v_MaterialIndex;



void main()
{
    uint meshIndex = gl_BaseInstance;
    GpuStaticMesh mesh = meshes[meshIndex];
    GpuVertex vertex = vertices[gl_VertexID];
    
    if (lights.length() == 0)
    {
        gl_Position = vec4(0.0);
        return;
    }
    
    GpuLight mainLight = lights[0];
    
    vec3 worldPos = vec3(mesh.m_Transform * vec4(vertex.m_Position, 1.0));
    gl_Position = mainLight.m_LightSpaceMatrix * vec4(worldPos, 1.0);
    
    v_TexCoord = vertex.m_TexCoord;
    v_MeshIndex = meshIndex;
    v_MaterialIndex = mesh.m_MaterialIndex;
}