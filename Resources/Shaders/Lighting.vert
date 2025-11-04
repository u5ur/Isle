// Lighting.vert
#version 460 core

layout(location = 0) in vec3 In_Pos;
layout(location = 1) in vec2 In_TexCoord;

layout(location = 0) out vec2 Out_TexCoord;

void main()
{
    Out_TexCoord = In_TexCoord;
    gl_Position = vec4(In_Pos, 1.0);
}
