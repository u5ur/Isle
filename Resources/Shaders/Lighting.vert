// Lighting.vert
#version 460 core

layout(location = 0) in vec3 In_Pos;
layout(location = 1) in vec2 In_TexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = In_TexCoord;
    gl_Position = vec4(In_Pos, 1.0);
}
