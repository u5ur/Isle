// Buffers.glsl

layout(std430, binding = 0) readonly buffer VertexBuffer { GpuVertex vertices[]; };
layout(std430, binding = 1) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 2) readonly buffer MaterialBuffer { GpuMaterial materials[]; };
layout(std430, binding = 3) readonly buffer MeshBuffer { GpuStaticMesh meshes[]; };
layout(std430, binding = 4) readonly buffer LightBuffer { GpuLight lights[]; };
layout(std140, binding = 5) uniform CameraBuffer { GpuCamera camera; };
layout(std430, binding = 6) readonly buffer TextureHandleBuffer { uint64_t textureHandles[]; };
