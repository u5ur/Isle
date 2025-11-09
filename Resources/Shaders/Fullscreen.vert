// Fullscreen.vert
#version 460 core
layout(location = 0) out vec2 TexCoord;

const vec2 quadVerts[3] = vec2[3]
(
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

void main() 
{
    gl_Position = vec4(quadVerts[gl_VertexID], 0.0, 1.0);
    TexCoord = (quadVerts[gl_VertexID] + 1.0) * 0.5;
}
