// Pipeline.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Structs/GpuStructs.h>
#include <Core/Graphics/GfxBuffer/GfxBuffer.h>
#include <Core/Graphics/Passes/FowardPass.h>

namespace Isle
{
    class Camera;
    class StaticMesh;
    class Mesh;
    class Material;

    class Pipeline : public Component
    {
    public:
        GfxBuffer* m_VertexBuffer;
        GfxBuffer* m_IndexBuffer;
        GfxBuffer* m_MaterialBuffer;
        GfxBuffer* m_CameraBuffer;
        GfxBuffer* m_StaticMeshBuffer;
        GfxBuffer* m_DrawCommandBuffer;
        GfxBuffer* m_TextureBuffer;
        GfxBuffer* m_DummyVAO;

        ForwardPass* m_ForwardPass;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        void AddIndexBuffer(std::vector<unsigned int> indicies);
        void AddVertexBuffer(std::vector<GpuVertex> vertex);

        void AddStaticMesh(StaticMesh* mesh);
        void AddMaterial(Material* material);

        void SetCamera(Camera* camera);

        void AddDrawCommand(Mesh* mesh);
    };
}
