// Forward.frag
#version 460 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

#include "Common\Common.glsl"

layout(location = 0) in vec3 in_WorldPos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) flat in uint in_MaterialIndex;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(1, 1, 1, 1);
}