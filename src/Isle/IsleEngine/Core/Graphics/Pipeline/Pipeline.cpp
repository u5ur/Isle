#include "Pipeline.h"
#include <Core/Light/Light.h>
#include <Core/Camera/Camera.h>
#include <Core/Graphics/Structs/GpuStructs.h>
#include <Core/Graphics/Material/Material.h>
#include <Core/Graphics/Mesh/StaticMesh.h>

namespace Isle
{
    void Pipeline::Start()
    {
        // CHANGED: Create buffers with New<>
        m_VertexBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::STORAGE);
        m_IndexBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::STORAGE);
        m_MaterialBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::STORAGE);
        m_CameraBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::UNIFORM, sizeof(GpuCamera));
        m_LightBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::STORAGE);
        m_StaticMeshBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::STORAGE);
        m_DrawCommandBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::INDIRECT_DRAW);
        m_TextureBuffer = New<GfxBuffer>(GFX_BUFFER_TYPE::STORAGE);
        m_DummyVAO = New<GfxBuffer>(GFX_BUFFER_TYPE::VERTEX, 0);
        m_DummyVAO->SetIndexBuffer(m_IndexBuffer.Get());

        // Passes owned by pipeline - raw pointers OK
        m_ForwardPass = new ForwardPass();
        m_ForwardPass->Start();

        m_GeometryPass = new GeometryPass();
        m_GeometryPass->Start();

        m_LightingPass = new LightingPass();
        m_LightingPass->Start();

        m_ShadowPass = new ShadowPass();
        m_ShadowPass->Start();

        m_VoxelPass = new VoxelPass();
        m_VoxelPass->Start();

        m_FullscreenQuad = new FullscreenQuad();
    }

    void Pipeline::Update()
    {
        m_VertexBuffer->Upload();
        m_IndexBuffer->Upload();
        m_MaterialBuffer->Upload();
        m_CameraBuffer->Upload();
        m_LightBuffer->Upload();
        m_StaticMeshBuffer->Upload();
        m_DrawCommandBuffer->Upload();

        m_VertexBuffer->Bind(0);
        m_IndexBuffer->Bind(1);
        m_MaterialBuffer->Bind(2);
        m_StaticMeshBuffer->Bind(3);
        m_LightBuffer->Bind(4);
        m_CameraBuffer->Bind(5);

        if (m_VoxelPass)
        {
            m_VoxelPass->Bind();
            Draw();
            m_VoxelPass->Unbind();

            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            m_VoxelPass->BuildVoxels();
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            m_VoxelPass->GenerateMipmaps();
            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        }

        if (m_ShadowPass)
        {
            m_ShadowPass->Bind();
            Draw();
            m_ShadowPass->Unbind();
        }

        if (m_GeometryPass)
        {
            m_GeometryPass->Bind();
            Draw();
            m_GeometryPass->Unbind();
        }

        if (m_LightingPass)
        {
            m_LightingPass->Bind();

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::COLOR)->Bind(6);
            m_LightingPass->GetShader()->SetInt("u_GColor", 6);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::NORMAL)->Bind(7);
            m_LightingPass->GetShader()->SetInt("u_GNormal", 7);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::POSITION)->Bind(8);
            m_LightingPass->GetShader()->SetInt("u_GPosition", 8);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::MATERIAL)->Bind(9);
            m_LightingPass->GetShader()->SetInt("u_GMaterial", 9);

            m_ShadowPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::SHADOW_MAP)->Bind(10);
            m_LightingPass->GetShader()->SetInt("u_ShadowMap", 10);

            m_VoxelPass->m_VoxelRadiance->Bind(11);
            m_LightingPass->GetShader()->SetInt("u_VoxelRadiance", 11);

            m_VoxelPass->m_VoxelNormal->Bind(12);
            m_LightingPass->GetShader()->SetInt("u_VoxelNormal", 12);

            m_LightingPass->m_Shader->SetIVec3("u_Resolution", m_VoxelPass->m_Resolution);
            m_LightingPass->m_Shader->SetIVec3("u_GridMin", m_VoxelPass->m_GridMin);
            m_LightingPass->m_Shader->SetIVec3("u_GridMax", m_VoxelPass->m_GridMax);
            m_LightingPass->m_Shader->SetInt("u_MipCount", m_VoxelPass->m_MipCount);
            m_LightingPass->m_Shader->SetVec3("u_CellSize", m_VoxelPass->m_CellSize);

            if (m_FullscreenQuad)
                m_FullscreenQuad->Draw();

            m_LightingPass->Unbind();
            m_LightingPass->BlitToScreen();
        }
    }

    void Pipeline::Destroy()
    {
        delete m_ForwardPass;
        delete m_GeometryPass;
        delete m_LightingPass;
        delete m_ShadowPass;
        delete m_VoxelPass;
        delete m_FullscreenQuad;
    }

    void Pipeline::Draw()
    {
        m_DummyVAO->Bind();
        m_DrawCommandBuffer->Bind();

        glMultiDrawElementsIndirect(
            GL_TRIANGLES,
            GL_UNSIGNED_INT,
            nullptr,
            m_DrawCommandBuffer->GetSize() / sizeof(GpuDrawCommand),
            sizeof(GpuDrawCommand)
        );

        m_DrawCommandBuffer->Unbind();
        m_DummyVAO->Unbind();
    }

    void Pipeline::AddIndexBuffer(std::vector<unsigned int> indices)
    {
        m_IndexBuffer->AddRange(indices);
    }

    void Pipeline::AddVertexBuffer(std::vector<GpuVertex> vertex)
    {
        m_VertexBuffer->AddRange(vertex);
    }

    void Pipeline::AddDrawCommand(Mesh* mesh)
    {
        GpuDrawCommand GDrawCmd{};
        GDrawCmd.m_Count = mesh->GetIndices().size();
        GDrawCmd.m_InstanceCount = 1;
        GDrawCmd.m_FirstIndex = m_IndexBuffer->GetSize() / sizeof(unsigned int);
        GDrawCmd.m_BaseVertex = 0;
        GDrawCmd.m_BaseInstance = m_StaticMeshBuffer->GetSize() / sizeof(GpuStaticMesh);
        m_DrawCommandBuffer->Add<GpuDrawCommand>(GDrawCmd);
    }

    void Pipeline::AddStaticMesh(StaticMesh* mesh)
    {
        m_StaticMeshBuffer->Add<GpuStaticMesh>(mesh->GetGpuStaticMesh());
        AddDrawCommand(mesh);
        AddIndexBuffer(mesh->GetIndices());
        AddVertexBuffer(mesh->GetVertices());

        auto material = mesh->GetMaterial();
        if (material)
            AddMaterial(material);
    }

    void Pipeline::AddMaterial(Material* material)
    {
        m_MaterialBuffer->Add<GpuMaterial>(material->GetGpuMaterial());
    }

    void Pipeline::SetCamera(Camera* camera)
    {
        m_CameraBuffer->SetData<GpuCamera>({ camera->GetCpuCamera() });
    }

    void Pipeline::AddLight(Light* light)
    {
        if (auto dirLight = dynamic_cast<DirectionalLight*>(light))
        {
            m_LightBuffer->Add<GpuLight>(dirLight->ToGpuLight());
        }
    }

    int Pipeline::GetNumVertices()
    {
        return m_VertexBuffer->GetSize() / sizeof(GpuVertex);
    }

    int Pipeline::GetNumIndicies()
    {
        return m_IndexBuffer->GetSize() / sizeof(unsigned int);
    }

    int Pipeline::GetNumLights()
    {
        return m_LightBuffer->GetSize() / sizeof(GpuLight);
    }

    int Pipeline::GetNumMaterials()
    {
        return m_MaterialBuffer->GetSize() / sizeof(GpuMaterial);
    }

    int Pipeline::GetNumTextures()
    {
        return m_TextureBuffer->GetSize() / sizeof(uint64_t);
    }

    int Pipeline::GetNumStaticMeshes()
    {
        return m_StaticMeshBuffer->GetSize() / sizeof(GpuStaticMesh);
    }
}