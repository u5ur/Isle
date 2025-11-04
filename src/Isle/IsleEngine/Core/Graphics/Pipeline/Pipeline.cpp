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
        m_VertexBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE);
        m_IndexBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE);
        m_MaterialBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE);
        m_CameraBuffer = new GfxBuffer(GFX_BUFFER_TYPE::UNIFORM, sizeof(GpuCamera));
        m_StaticMeshBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE);
        m_DrawCommandBuffer = new GfxBuffer(GFX_BUFFER_TYPE::INDIRECT_DRAW);
        m_TextureBuffer = new GfxBuffer(GFX_BUFFER_TYPE::STORAGE);
        m_DummyVAO = new GfxBuffer(GFX_BUFFER_TYPE::VERTEX, 0);
        m_DummyVAO->SetIndexBuffer(m_IndexBuffer);

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

        m_VertexBuffer->Bind(0);
        m_IndexBuffer->Bind(1);
        m_MaterialBuffer->Bind(2);
        m_StaticMeshBuffer->Bind(3);
        m_CameraBuffer->Bind(4);

        if (m_ForwardPass)
        {
            m_ForwardPass->Bind();

            m_DummyVAO->Bind();
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_DrawCommandBuffer->GetId());

            GLsizei drawCount = m_DrawCommandBuffer->GetSize() / sizeof(GpuDrawCommand);

            glMultiDrawElementsIndirect(
                GL_TRIANGLES,
                GL_UNSIGNED_INT,
                nullptr,
                drawCount,
                sizeof(GpuDrawCommand)
            );

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            glBindVertexArray(0);

            m_ForwardPass->Unbind();
            m_ForwardPass->BlitToScreen();
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
        GDrawCmd.m_BaseVertex = mesh->GetVertexOffset();
        GDrawCmd.m_BaseInstance = m_StaticMeshBuffer->GetSize() / sizeof(GpuStaticMesh);

        ISLE_LOG("Draw Command: Count=%d, FirstIndex=%d, BaseVertex=%d, BaseInstance=%d\n",
            GDrawCmd.m_Count, GDrawCmd.m_FirstIndex, GDrawCmd.m_BaseVertex, GDrawCmd.m_BaseInstance);

        m_DrawCommandBuffer->Add<GpuDrawCommand>(GDrawCmd);
    }

    void Pipeline::AddStaticMesh(StaticMesh* mesh)
    {
        uint32_t VertexBuffer_Size = m_VertexBuffer->GetSize() / sizeof(GpuVertex);
        uint32_t IndexBuffer_Size = m_IndexBuffer->GetSize() / sizeof(unsigned int);
        uint32_t materialIndex = m_MaterialBuffer->GetSize() / sizeof(GpuMaterial);
        uint32_t MeshBuffer_Size = m_StaticMeshBuffer->GetSize() / sizeof(GpuStaticMesh);

        GpuStaticMesh gpuMesh = mesh->GetGpuStaticMesh();
        gpuMesh.m_MaterialIndex = materialIndex;
        gpuMesh.m_Transform = mesh->GetWorldMatrix();
        gpuMesh.m_NormalMatrix = glm::transpose(glm::inverse(gpuMesh.m_Transform));
        gpuMesh.m_VertexOffset = VertexBuffer_Size;
        gpuMesh.m_IndexOffset = IndexBuffer_Size;
        gpuMesh.m_IndexCount = mesh->GetIndices().size();
        gpuMesh.m_UseViewModel = 0;

        GpuDrawCommand drawCmd{};
        drawCmd.m_Count = mesh->GetIndexCount();
        drawCmd.m_InstanceCount = 1;
        drawCmd.m_FirstIndex = IndexBuffer_Size;
        drawCmd.m_BaseVertex = 0;
        drawCmd.m_BaseInstance = MeshBuffer_Size;

        m_StaticMeshBuffer->Add<GpuStaticMesh>(gpuMesh);
        m_MaterialBuffer->Add<GpuMaterial>(mesh->GetMaterial()->GetGpuMaterial());
        m_VertexBuffer->AddRange(mesh->GetVertices());
        m_IndexBuffer->AddRange(mesh->GetIndices());
        m_DrawCommandBuffer->Add<GpuDrawCommand>(drawCmd);
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
