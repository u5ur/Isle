#include "VoxelPass.h"

namespace Isle
{
    void VoxelPass::Start()
    {
        m_VoxelRadiance = New<Texture3D>();
        m_VoxelRadiance->Create(m_Resolution.x, m_Resolution.y, m_Resolution.z, TEXTURE3D_FORMAT::RGBA16F, nullptr, true);
        m_VoxelRadiance->SetMinFilter(TEXTURE3D_FILTER::LINEAR_MIPMAP_LINEAR);
        m_VoxelRadiance->SetMagFilter(TEXTURE3D_FILTER::LINEAR);
        m_VoxelRadiance->SetWrap(TEXTURE3D_WRAP::CLAMP_TO_BORDER, TEXTURE3D_WRAP::CLAMP_TO_BORDER, TEXTURE3D_WRAP::CLAMP_TO_BORDER);
        m_VoxelRadiance->SetBorderColor(glm::vec4(0.0f));

        m_AtomicRadiance = New<Texture3D>();
        m_AtomicRadiance->Create(m_Resolution.x, m_Resolution.y, m_Resolution.z, TEXTURE3D_FORMAT::RGBA16F, nullptr, true);
        m_AtomicRadiance->SetMinFilter(TEXTURE3D_FILTER::NEAREST);
        m_AtomicRadiance->SetMagFilter(TEXTURE3D_FILTER::LINEAR);

        m_VoxelNormal = New<Texture3D>();
        m_VoxelNormal->Create(m_Resolution.x, m_Resolution.y, m_Resolution.z, TEXTURE3D_FORMAT::RGBA16F, nullptr, true);
        m_VoxelNormal->SetMinFilter(TEXTURE3D_FILTER::LINEAR_MIPMAP_LINEAR);
        m_VoxelNormal->SetMagFilter(TEXTURE3D_FILTER::LINEAR);
        m_VoxelNormal->SetBorderColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

        m_AtomicNormal = New<Texture3D>();
        m_AtomicNormal->Create(m_Resolution.x, m_Resolution.y, m_Resolution.z, TEXTURE3D_FORMAT::RGBA16F, nullptr, true);
        m_AtomicNormal->SetMinFilter(TEXTURE3D_FILTER::NEAREST);
        m_AtomicNormal->SetMagFilter(TEXTURE3D_FILTER::LINEAR);

        m_AtomicCounter = New<Texture3D>();
        m_AtomicCounter->Create(m_Resolution.x, m_Resolution.y, m_Resolution.z, TEXTURE3D_FORMAT::R32UI, nullptr, true);
        m_AtomicCounter->SetMinFilter(TEXTURE3D_FILTER::NEAREST);
        m_AtomicCounter->SetMagFilter(TEXTURE3D_FILTER::NEAREST);

        m_AtomicRadiance->Clear(glm::vec4(0.0f));
        m_AtomicCounter->Clear(glm::vec4(0.0f));

        m_Shader = New<Shader>();
        m_Shader->LoadFromFile(SHADER_TYPE::VERTEX, "Resources\\Shaders\\Voxel\\Voxelize.vert");
        m_Shader->LoadFromFile(SHADER_TYPE::GEOMETRY, "Resources\\Shaders\\Voxel\\Voxelize.geom");
        m_Shader->LoadFromFile(SHADER_TYPE::FRAGMENT, "Resources\\Shaders\\Voxel\\Voxelize.frag");
        m_Shader->Link();

        m_MipmapShader = New<Shader>();
        m_MipmapShader->LoadFromFile(SHADER_TYPE::COMPUTE, "Resources\\Shaders\\Voxel\\GenerateMipmaps.comp");
        m_MipmapShader->Link();

        m_BuildShader = New<Shader>();
        m_BuildShader->LoadFromFile(SHADER_TYPE::COMPUTE, "Resources\\Shaders\\Voxel\\BuildVoxels.comp");
        m_BuildShader->Link();

        m_CellSize = glm::vec3((m_GridMax - m_GridMin)) / glm::vec3(m_Resolution);
        m_MipCount = static_cast<int>(glm::floor(glm::log2(static_cast<float>(m_Resolution.x)))) + 1;
        m_MaxMipLevel = m_MipCount;

        SetupViewport();
    }

    void VoxelPass::Update() {}

    void VoxelPass::Bind()
    {
        glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
        SetViewport();

        m_Shader->Bind();
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        m_AtomicRadiance->BindAsImage(0, GL_READ_WRITE, 0);
        m_AtomicNormal->BindAsImage(1, GL_READ_WRITE, 0);
        m_AtomicCounter->BindAsImage(2, GL_READ_WRITE, 0);

        m_Shader->SetIVec3("u_Resolution", m_Resolution);
        m_Shader->SetIVec3("u_GridMin", m_GridMin);
        m_Shader->SetIVec3("u_GridMax", m_GridMax);
        m_Shader->SetInt("u_MipCount", m_MipCount);
        m_Shader->SetVec3("u_CellSize", m_CellSize);
    }

    void VoxelPass::Unbind()
    {
        glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    void VoxelPass::Destroy()
    {
        if (m_VoxelRadiance) {
            m_VoxelRadiance->Destroy();
        }

        if (m_VoxelNormal) {
            m_VoxelNormal->Destroy();
        }

        if (m_AtomicRadiance) {
            m_AtomicRadiance->Destroy();
        }

        if (m_AtomicCounter) {
            m_AtomicCounter->Destroy();
        }
    }

    void VoxelPass::GenerateMipmaps()
    {
        if (m_MipmapShader)
        {
            glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
            glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glBindImageTexture(2, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

            m_MipmapShader->Bind();

            for (int mip = 1; mip < m_MaxMipLevel; mip++)
            {
                glm::ivec3 mipRes = m_Resolution / (1 << mip);
                mipRes = glm::max(mipRes, glm::ivec3(1));

                m_VoxelRadiance->BindAsImage(0, GL_WRITE_ONLY, mip);
                m_VoxelNormal->BindAsImage(1, GL_WRITE_ONLY, mip);
                m_VoxelRadiance->BindAsImage(2, GL_READ_ONLY, mip - 1);
                m_VoxelNormal->BindAsImage(3, GL_READ_ONLY, mip - 1);

                m_MipmapShader->SetInt("u_MipLevel", mip);
                m_MipmapShader->SetIVec3("u_RegionMin", glm::ivec3(0));
                m_MipmapShader->SetIVec3("u_RegionMax", mipRes - glm::ivec3(1));

                glm::ivec3 groupCount = (mipRes + glm::ivec3(3)) / glm::ivec3(4);
                glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);

                if (mip < m_MaxMipLevel - 1)
                    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            }
        }
    }

    void VoxelPass::BuildVoxels()
    {
        if (m_BuildShader)
        {
            m_BuildShader->Bind();
            m_AtomicRadiance->BindAsImage(0, GL_READ_WRITE, 0);
            m_AtomicNormal->BindAsImage(1, GL_READ_WRITE, 0);
            m_VoxelRadiance->BindAsImage(2, GL_WRITE_ONLY, 0);
            m_VoxelNormal->BindAsImage(3, GL_WRITE_ONLY, 0);
            m_AtomicCounter->BindAsImage(4, GL_READ_WRITE, 0);

            m_BuildShader->SetIVec3("u_Resolution", m_Resolution);
            m_BuildShader->SetIVec3("u_GridMin", m_GridMin);
            m_BuildShader->SetIVec3("u_GridMax", m_GridMax);
            m_BuildShader->SetVec3("u_CellSize", m_CellSize);

            glm::ivec3 groupCount = (m_Resolution + glm::ivec3(7)) / glm::ivec3(8);
            glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);

            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }
    }

    void VoxelPass::SetupViewport()
    {
        glViewportSwizzleNV(0, GL_VIEWPORT_SWIZZLE_POSITIVE_X_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_Y_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_Z_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_W_NV);

        glViewportSwizzleNV(1, GL_VIEWPORT_SWIZZLE_POSITIVE_X_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_Z_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_Y_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_W_NV);

        glViewportSwizzleNV(2, GL_VIEWPORT_SWIZZLE_POSITIVE_Z_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_Y_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_X_NV,
            GL_VIEWPORT_SWIZZLE_POSITIVE_W_NV);
    }

    void VoxelPass::SetViewport()
    {
        glViewportIndexedf(0, 0, 0, m_Resolution.x, m_Resolution.y);
        glViewportIndexedf(1, 0, 0, m_Resolution.y, m_Resolution.z);
        glViewportIndexedf(2, 0, 0, m_Resolution.z, m_Resolution.x);
    }
}