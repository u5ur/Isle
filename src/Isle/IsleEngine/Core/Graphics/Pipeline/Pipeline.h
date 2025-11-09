#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Structs/GpuStructs.h>
#include <Core/Graphics/GfxBuffer/GfxBuffer.h>
#include <Core/Graphics/Passes/FowardPass.h>
#include <Core/Graphics/Passes/GeometryPass.h>
#include <Core/Graphics/Passes/LightingPass.h>
#include <Core/Graphics/Passes/ShadowPass.h>
#include <Core/Graphics/Passes/FullscreenQuad.h>
#include <Core/Graphics/Passes/VoxelPass.h>
#include <Core/Graphics/Passes/CompositePass.h>
#include <Core/Graphics/Passes/SelectionPass.h>

namespace Isle
{
    class Camera;
    class StaticMesh;
    class Mesh;
    class Light;
    class Material;

    class ISLEENGINE_API Pipeline : public Component
    {
    private:
        Ref<GfxBuffer> m_VertexBuffer;
        Ref<GfxBuffer> m_IndexBuffer;
        Ref<GfxBuffer> m_MaterialBuffer;
        Ref<GfxBuffer> m_CameraBuffer;
        Ref<GfxBuffer> m_LightBuffer;
        Ref<GfxBuffer> m_StaticMeshBuffer;
        Ref<GfxBuffer> m_DrawCommandBuffer;
        Ref<GfxBuffer> m_TextureBuffer;
        Ref<GfxBuffer> m_DummyVAO;

        ForwardPass* m_ForwardPass;
        GeometryPass* m_GeometryPass;
        LightingPass* m_LightingPass;
        ShadowPass* m_ShadowPass;
        VoxelPass* m_VoxelPass;
        CompositePass* m_CompositePass;
        SelectionPass* m_SelectionPass;
        FullscreenQuad* m_FullscreenQuad;

        std::unordered_map<GLuint, uint32_t> m_TextureToIndex;
        std::unordered_map<Material*, uint32_t> m_MaterialToIndex;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        void Draw();
        void DrawSelected();

        void AddIndexBuffer(std::vector<unsigned int> indices);
        void AddVertexBuffer(std::vector<GpuVertex> vertex);
        void AddStaticMesh(StaticMesh* mesh);
        void AddMaterial(Material* material);
        void AddMaterialTexture(Material* material, std::string name);

        void UpdateStaticMesh(StaticMesh* mesh);
        void UpdateMaterial(Material* material);
        void UpdateLight(Light* light);


        void AddLight(Light* light);
        void AddDrawCommand(Mesh* mesh);
        void SetCamera(Camera* camera);

        void SelectMesh(Mesh* selectedMesh, bool state);

        Ref<GfxBuffer> GetStaticMeshBuffer();

        int GetNumVertices();
        int GetNumIndicies();
        int GetNumMaterials();
        int GetNumLights();
        int GetNumTextures();
        int GetNumStaticMeshes();


        void Clear();
        Ref<Texture> GetFinalOutput();
    };
}