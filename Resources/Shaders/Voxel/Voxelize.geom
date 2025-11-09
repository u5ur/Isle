// Voxelize.geom

#version 460 core
#extension GL_NV_viewport_swizzle : enable

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    flat uint MaterialIndex;
} gs_in[];

out FragmentData
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    flat uint MaterialIndex;
    flat vec3 TriNormal;
} gs_out;

void main()
{
    vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normalWeights = abs(cross(p1, p2));
    
    int dominantAxis = normalWeights.y > normalWeights.x ? 1 : 0;
    dominantAxis = normalWeights.z > normalWeights[dominantAxis] ? 2 : dominantAxis;
    
    vec3 edge1 = gs_in[1].FragPos - gs_in[0].FragPos;
    vec3 edge2 = gs_in[2].FragPos - gs_in[0].FragPos;
    vec3 triNormal = normalize(cross(edge1, edge2));
    
    gl_ViewportIndex = 2 - dominantAxis;
    
    for (int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        
        gs_out.FragPos = gs_in[i].FragPos;
        gs_out.Normal = gs_in[i].Normal;
        gs_out.TexCoord = gs_in[i].TexCoord;
        gs_out.MaterialIndex = gs_in[i].MaterialIndex;
        gs_out.TriNormal = triNormal;
        
        EmitVertex();
    }
    
    EndPrimitive();
}