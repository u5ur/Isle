// Pipeline.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Structs/GpuStructs.h>
#include <Core/Graphics/GfxBuffer/GfxBuffer.h>
#include <Core/Graphics/Passes/FowardPass.h>
#include <Core/Graphics/Passes/GeometryPass.h>
#include <Core/Graphics/Passes/LightingPass.h>
#include <Core/Graphics/Passes/FullscreenQuad.h>

namespace Isle
{
    class Camera;
    class StaticMesh;
    class Mesh;
    class Light;
    class Material;

    class Pipeline : public Component
    {
    private:
        GfxBuffer* m_VertexBuffer;
        GfxBuffer* m_IndexBuffer;
        GfxBuffer* m_MaterialBuffer;
        GfxBuffer* m_CameraBuffer;
        GfxBuffer* m_LightBuffer;
        GfxBuffer* m_StaticMeshBuffer;
        GfxBuffer* m_DrawCommandBuffer;
        GfxBuffer* m_TextureBuffer;
        GfxBuffer* m_DummyVAO;

        ForwardPass* m_ForwardPass;
        GeometryPass* m_GeometryPass;
        LightingPass* m_LightingPass;

        FullscreenQuad* m_FullscreenQuad;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        void Draw();

        void AddIndexBuffer(std::vector<unsigned int> indicies);
        void AddVertexBuffer(std::vector<GpuVertex> vertex);
        void AddStaticMesh(StaticMesh* mesh);
        void AddMaterial(Material* material);
        void AddLight(Light* light);
        void AddDrawCommand(Mesh* mesh);

        void SetCamera(Camera* camera);

        int GetNumVertices();
        int GetNumIndicies();
        int GetNumMaterials();
        int GetNumLights();
        int GetNumTextures();
        int GetNumStaticMeshes();
    };
}
