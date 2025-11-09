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

        m_CompositePass = new CompositePass();
        m_CompositePass->Start();

        m_SelectionPass = new SelectionPass();
        m_SelectionPass->Start();

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
        m_TextureBuffer->Upload();

        m_VertexBuffer->Bind(0);
        m_IndexBuffer->Bind(1);
        m_MaterialBuffer->Bind(2);
        m_StaticMeshBuffer->Bind(3);
        m_LightBuffer->Bind(4);
        m_CameraBuffer->Bind(5);
        m_TextureBuffer->Bind(6);

        if (m_VoxelPass)
        {
            m_VoxelPass->Bind();

            m_ShadowPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::SHADOW_MAP)->Bind(7);
            m_VoxelPass->GetShader()->SetInt("u_ShadowMap", 7);

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

        if (m_SelectionPass)
        {
            m_SelectionPass->Bind();
            DrawSelected();
            m_SelectionPass->Unbind();
        }

        if (m_LightingPass)
        {
            m_LightingPass->Bind();

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::COLOR)->Bind(7);
            m_LightingPass->GetShader()->SetInt("u_GColor", 7);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::NORMAL)->Bind(8);
            m_LightingPass->GetShader()->SetInt("u_GNormal", 8);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::POSITION)->Bind(9);
            m_LightingPass->GetShader()->SetInt("u_GPosition", 9);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::MATERIAL)->Bind(10);
            m_LightingPass->GetShader()->SetInt("u_GMaterial", 10);

            m_ShadowPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::SHADOW_MAP)->Bind(11);
            m_LightingPass->GetShader()->SetInt("u_ShadowMap", 11);

            if (m_FullscreenQuad)
                m_FullscreenQuad->Draw();

            m_LightingPass->Unbind();
        }

        if (m_CompositePass)
        {
            m_CompositePass->Bind();

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::COLOR)->Bind(7);
            m_CompositePass->GetShader()->SetInt("u_GColor", 7);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::NORMAL)->Bind(8);
            m_CompositePass->GetShader()->SetInt("u_GNormal", 8);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::POSITION)->Bind(9);
            m_CompositePass->GetShader()->SetInt("u_GPosition", 9);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::MATERIAL)->Bind(10);
            m_CompositePass->GetShader()->SetInt("u_GMaterial", 10);

            m_ShadowPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::SHADOW_MAP)->Bind(11);
            m_CompositePass->GetShader()->SetInt("u_ShadowMap", 11);

            m_LightingPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::COLOR)->Bind(12);
            m_CompositePass->GetShader()->SetInt("u_DirectLighting", 12);

            m_SelectionPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::SELECTION)->Bind(13);
            m_CompositePass->GetShader()->SetInt("u_Selection", 13);

            m_VoxelPass->m_VoxelRadiance->Bind(14);
            m_CompositePass->GetShader()->SetInt("u_VoxelRadiance", 14);

            m_VoxelPass->m_VoxelNormal->Bind(15);
            m_CompositePass->GetShader()->SetInt("u_VoxelNormal", 15);

            m_GeometryPass->GetFrameBuffer()->GetAttachment(ATTACHMENT_TYPE::DEPTH)->Bind(16);
            m_CompositePass->GetShader()->SetInt("u_DepthBuffer", 16);

            m_CompositePass->m_Shader->SetIVec3("u_Resolution", m_VoxelPass->m_Resolution);
            m_CompositePass->m_Shader->SetIVec3("u_GridMin", m_VoxelPass->m_GridMin);
            m_CompositePass->m_Shader->SetIVec3("u_GridMax", m_VoxelPass->m_GridMax);
            m_CompositePass->m_Shader->SetInt("u_MipCount", m_VoxelPass->m_MipCount);
            m_CompositePass->m_Shader->SetVec3("u_CellSize", m_VoxelPass->m_CellSize);

            if (m_FullscreenQuad)
                m_FullscreenQuad->Draw();

            m_CompositePass->Unbind();
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

    void Pipeline::Clear()
    {
        if (m_VertexBuffer) m_VertexBuffer->Clear();
        if (m_IndexBuffer) m_IndexBuffer->Clear();
        if (m_MaterialBuffer) m_MaterialBuffer->Clear();
        if (m_LightBuffer) m_LightBuffer->Clear();
        if (m_StaticMeshBuffer) m_StaticMeshBuffer->Clear();
        if (m_DrawCommandBuffer) m_DrawCommandBuffer->Clear();
        if (m_TextureBuffer) m_TextureBuffer->Clear();
        m_TextureToIndex.clear();
        m_MaterialToIndex.clear();
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

    void Pipeline::DrawSelected()
    {
        m_DummyVAO->Bind();
        m_DrawCommandBuffer->Bind();

        std::vector<GpuStaticMesh> staticMeshes = m_StaticMeshBuffer->GetData<GpuStaticMesh>();
        std::vector<GpuDrawCommand> drawCommands = m_DrawCommandBuffer->GetData<GpuDrawCommand>();

        for (size_t i = 0; i < drawCommands.size(); i++)
        {
            uint32_t baseInstance = drawCommands[i].m_BaseInstance;

            if (staticMeshes[baseInstance].m_Selected)
            {
                const GpuDrawCommand& cmd = drawCommands[i];
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    cmd.m_Count,
                    GL_UNSIGNED_INT,
                    reinterpret_cast<void*>(cmd.m_FirstIndex * sizeof(uint32_t)),
                    cmd.m_InstanceCount,
                    cmd.m_BaseVertex,
                    cmd.m_BaseInstance
                );
            }
        }

        m_DrawCommandBuffer->Unbind();
        m_DummyVAO->Unbind();
    }

    Ref<Texture> Pipeline::GetFinalOutput()
    {
        if (m_CompositePass)
        {
            return m_CompositePass->GetOutputTexture();
        }
        return nullptr;
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
        GDrawCmd.m_BaseVertex = m_VertexBuffer->GetSize() / sizeof(GpuVertex);
        GDrawCmd.m_BaseInstance = m_StaticMeshBuffer->GetSize() / sizeof(GpuStaticMesh);
        m_DrawCommandBuffer->Add<GpuDrawCommand>(GDrawCmd);
    }

    void Pipeline::AddStaticMesh(StaticMesh* mesh)
    {
        auto material = mesh->GetMaterial();
        uint32_t materialIndex = -1;

        if (material)
        {
            AddMaterialTexture(material, "BaseColor");
            AddMaterialTexture(material, "Emissive");
            AddMaterialTexture(material, "MetallicRoughness");
            AddMaterialTexture(material, "Occlusion");
            AddMaterialTexture(material, "Normal");

            auto it = m_MaterialToIndex.find(material);
            if (it != m_MaterialToIndex.end())
            {
                materialIndex = it->second;
            }
            else
            {
                GpuMaterial gpuMat = material->GetGpuMaterial();
                materialIndex = m_MaterialBuffer->GetSize() / sizeof(GpuMaterial);
                m_MaterialBuffer->Add<GpuMaterial>(gpuMat);
                m_MaterialToIndex[material] = materialIndex;
            }
        }

        GpuStaticMesh gpuMesh = mesh->GetGpuStaticMesh();
        gpuMesh.m_MaterialIndex = materialIndex;

        AddDrawCommand(mesh);
        mesh->m_Id = GetNumStaticMeshes();
        m_StaticMeshBuffer->Add<GpuStaticMesh>(gpuMesh);
        AddIndexBuffer(mesh->GetIndices());
        AddVertexBuffer(mesh->GetVertices());
    }

    void Pipeline::UpdateLight(Light* light)
    {
        if (!light)
            return;

        if (light->m_Id == -1)
            return;

        auto currentData = m_LightBuffer->GetData<GpuLight>();
        if (light->m_Id >= currentData.size())
            return;

        GpuLight gpuLight{};
        if (auto dirLight = dynamic_cast<DirectionalLight*>(light))
        {
            gpuLight = dirLight->ToGpuLight();
        }
        else if (auto pointLight = dynamic_cast<PointLight*>(light))
        {
            gpuLight = pointLight->ToGpuLight();
        }
        else if (auto spotLight = dynamic_cast<SpotLight*>(light))
        {
            gpuLight = spotLight->ToGpuLight();
        }
        else
        {
            gpuLight = light->ToGpuLight();
        }

        size_t offsetInBytes = light->m_Id * sizeof(GpuLight);
        auto bufferData = m_LightBuffer->GetData<uint8_t>();

        if (offsetInBytes + sizeof(GpuLight) <= bufferData.size())
        {
            std::memcpy(
                bufferData.data() + offsetInBytes,
                &gpuLight,
                sizeof(GpuLight)
            );

            m_LightBuffer->SetData(bufferData);
        }
    }


    void Pipeline::AddMaterial(Material* material)
    {
        m_MaterialBuffer->Add<GpuMaterial>(material->GetGpuMaterial());
    }

    void Pipeline::UpdateStaticMesh(StaticMesh* mesh)
    {
        if (mesh->m_Id == -1)
            return;

        auto currentData = m_StaticMeshBuffer->GetData<GpuStaticMesh>();
        if (mesh->m_Id >= currentData.size())
            return;

        bool wasSelected = currentData[mesh->m_Id].m_Selected;

        GpuStaticMesh gpuMesh = mesh->GetGpuStaticMesh();
        gpuMesh.m_Selected = wasSelected;

        if (mesh->GetMaterial())
        {
            UpdateMaterial(mesh->GetMaterial());

            auto it = m_MaterialToIndex.find(mesh->GetMaterial());
            if (it != m_MaterialToIndex.end())
            {
                gpuMesh.m_MaterialIndex = it->second;
            }
        }

        size_t offsetInBytes = mesh->m_Id * sizeof(GpuStaticMesh);
        auto bufferData = m_StaticMeshBuffer->GetData<uint8_t>();

        if (offsetInBytes + sizeof(GpuStaticMesh) <= bufferData.size())
        {
            std::memcpy(
                bufferData.data() + offsetInBytes,
                &gpuMesh,
                sizeof(GpuStaticMesh)
            );

            m_StaticMeshBuffer->SetData(bufferData);
        }
    }

    void Pipeline::UpdateMaterial(Material* material)
    {
        if (!material)
            return;

        auto it = m_MaterialToIndex.find(material);
        if (it == m_MaterialToIndex.end())
            return;

        uint32_t materialIndex = it->second;
        GpuMaterial gpuMaterial = material->GetGpuMaterial();
        size_t offsetInBytes = materialIndex * sizeof(GpuMaterial);
        auto bufferData = m_MaterialBuffer->GetData<uint8_t>();

        if (offsetInBytes + sizeof(GpuMaterial) <= bufferData.size())
        {
            std::memcpy(
                bufferData.data() + offsetInBytes,
                &gpuMaterial,
                sizeof(GpuMaterial)
            );

            m_MaterialBuffer->SetData(bufferData);
        }
    }

    void Pipeline::AddMaterialTexture(Material* material, std::string name)
    {
        auto texture = material->GetTexture(name);
        if (!texture)
            return;

        auto it = m_TextureToIndex.find(texture->m_Id);
        if (it == m_TextureToIndex.end())
        {
            GLuint64 handle = texture->GetBindlessHandle();
            uint32_t index = m_TextureBuffer->GetSize() / sizeof(GLuint64);

            m_TextureBuffer->Add(handle);
            texture->m_BindlessIndex = index;
            m_TextureToIndex[texture->m_Id] = index;
        }
        else
        {
            texture->m_BindlessIndex = it->second;
        }
    }


    void Pipeline::SetCamera(Camera* camera)
    {
        if (!camera)
            return;

        m_CameraBuffer->SetData<GpuCamera>({ camera->GetCpuCamera() });
    }

    void Pipeline::AddLight(Light* light)
    {
        light->m_Id = GetNumLights();
        if (auto dirLight = dynamic_cast<DirectionalLight*>(light))
        {
            m_LightBuffer->Add<GpuLight>(dirLight->ToGpuLight());
        }
    }

    void Pipeline::SelectMesh(Mesh* selectedMesh, bool state)
    {
        if (!selectedMesh || selectedMesh->m_Id < 0)
            return;

        std::vector<GpuStaticMesh> currentData = m_StaticMeshBuffer->GetData<GpuStaticMesh>();

        if (selectedMesh->m_Id >= currentData.size())
        {
            return;
        }
        for (auto& mesh : currentData)
        {
            mesh.m_Selected = 0;
        }

        if (state)
        {
            currentData[selectedMesh->m_Id].m_Selected = 1;
        }

        m_StaticMeshBuffer->SetData(currentData);
    }


    Ref<GfxBuffer> Pipeline::GetStaticMeshBuffer()
    {
        return m_StaticMeshBuffer;
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