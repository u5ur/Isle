// Pipeline.cpp
#include "Pipeline.h"
#include <Core/Graphics/Structs/GpuStructs.h>
#include <Core/Camera/Camera.h>
#include <Core/Graphics/Material/Material.h>
#include <Core/Graphics/Mesh/StaticMesh.h>

namespace Isle
{
    void Pipeline::Start()
    {
        m_VertexBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE,  sizeof(GpuVertex));
        m_IndexBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE, sizeof(unsigned int));
        m_MaterialBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE, sizeof(GpuMaterial));
        m_CameraBuffer = new GfxBuffer(GFX_BUFFER_TYPE::UNIFORM, sizeof(GpuCamera));
        m_StaticMeshBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE, sizeof(GpuStaticMesh));
        m_DrawCommandBuffer = new GfxBuffer(GFX_BUFFER_TYPE::INDIRECT_DRAW, sizeof(GpuDrawCommand));
        m_TextureBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE, sizeof(uint64_t));
        m_DummyVAO = new GfxBuffer(GFX_BUFFER_TYPE::VERTEX, sizeof(float));

        m_ForwardPass = new ForwardPass();
        m_ForwardPass->Start();
    }

    void Pipeline::Update()
    {
        m_VertexBuffer->Upload();
        m_IndexBuffer->Upload();
        m_MaterialBuffer->Upload();
        m_CameraBuffer->Upload();
        m_StaticMeshBuffer->Upload();
        m_DrawCommandBuffer->Upload();
        m_TextureBuffer->Upload();

        m_VertexBuffer->Bind(0);
        m_IndexBuffer->Bind(1);
        m_MaterialBuffer->Bind(2);
        m_CameraBuffer->Bind(3);
        m_StaticMeshBuffer->Bind(4);
        m_DrawCommandBuffer->Bind(5);
        m_TextureBuffer->Bind(6);

        if (m_ForwardPass)
        {
            m_ForwardPass->Bind();
            m_DummyVAO->Bind();
            m_DrawCommandBuffer->Bind();

            glMultiDrawElementsIndirect(
                GL_TRIANGLES,
                GL_UNSIGNED_INT,
                nullptr,
                m_DrawCommandBuffer->GetSize(),
                sizeof(GpuDrawCommand)
            );

            m_ForwardPass->Unbind();
        }
    }

    void Pipeline::Destroy()
    {

    }

    void Pipeline::AddIndexBuffer(std::vector<unsigned int> indicies)
    {
        m_IndexBuffer->AddRange(indicies);
    }

    void Pipeline::AddVertexBuffer(std::vector<GpuVertex> vertex)
    {
        m_VertexBuffer->AddRange(vertex);
    }

    void Pipeline::AddDrawCommand(Mesh* mesh)
    {
        GpuDrawCommand GDrawCmd{};
        GDrawCmd.m_Count = mesh->GetIndexCount();
        GDrawCmd.m_InstanceCount = 1;
        GDrawCmd.m_FirstIndex = mesh->GetIndexOffset();
        GDrawCmd.m_BaseVertex = 0;
        GDrawCmd.m_BaseInstance = m_StaticMeshBuffer->GetSize();
        m_DrawCommandBuffer->Add<GpuDrawCommand>(GDrawCmd);
    }

    void Pipeline::AddStaticMesh(StaticMesh* mesh)
    {
        m_StaticMeshBuffer->Add<GpuStaticMesh>(mesh->GetGpuStaticMesh());
        m_MaterialBuffer->Add<GpuMaterial>(mesh->GetMaterial()->GetGpuMaterial());
        AddDrawCommand(mesh);
    }

    void Pipeline::AddMaterial(Material* material)
    {
        m_MaterialBuffer->Add<GpuMaterial>(material->GetGpuMaterial());
    }

    void Pipeline::SetCamera(Camera* camera)
    {
        m_CameraBuffer->SetData<GpuCamera>({ camera->GetCpuCamera() });
    }
}
