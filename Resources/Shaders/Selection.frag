// Selection.vert
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable
#include "Common/Common.glsl"

layout(location = 0) out float FragSelection;
layout(location = 0) flat in uint In_MeshIndex;

void main()
{
    GpuStaticMesh mesh = meshes[In_MeshIndex];
    FragSelection = (mesh.m_Selected > 0) ? 1.0 : 0.0;
}
