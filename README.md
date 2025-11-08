IsleEngine (WIP)

IsleEngine is a full-scale C++ game engine built entirely on modern OpenGL.

## Key Features

- Deferred and Forward Rendering
- Multi-Pass System
- GPU Based Rendering Architecture  
- Bindless Texture System
- PBR Materials
- Indirect Rendering and Instancing
- Blender/Unreal like editor
- HotLoad game directly from editor
- Memory management (wip)
- GLTF/GLB file support
- Global Illumination and Reflections using VoxelCone tracing

**ENGINE TODO**
- Serialization
- Frustum/Occlusion culling (run in compute)
- Temporal Anti Aliasing
- Screen Space Ambient Occlusion
- Screen Space Reflections
- Cascade Shadow Maps
- Bloom/Emissive Glow
- Animation and Skeletal Mesh Support  
- Scriptable Gameplay Layer
- Physics (using physx)
- Volumetric Lighting/Effects
- PostProcessing Effects
- Fix performance from updating gpu buffers and reduce memory usage on gpu

**EDITOR TODO**
- Serialization
- Asset Browser
- Fix crashing when hot loading
- Support all SceneComponents in Properties
- Command system (ex. copy and paste)
- Rendering presets (ex. no direct lighting)
- Debugging (stats, framebuffer views, etc)
