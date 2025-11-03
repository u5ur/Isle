#include "Structs.glsl"

layout(std430, binding = 0) readonly buffer VertexBuffer { GPU_Vertex vertices[]; };
layout(std430, binding = 1) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 2) readonly buffer MaterialBuffer { GPU_Material materials[]; };
layout(std430, binding = 3) readonly buffer MeshBuffer { GPU_Mesh meshes[]; };
layout(std140, binding = 4) uniform CameraBuffer { GPU_Camera camera; };
layout(std430, binding = 5) buffer DrawCommandBuffer { GPU_DrawCommand drawCommands[]; };
layout(std430, binding = 6) readonly buffer TextureHandleBuffer { uint64_t textureHandles[]; };
