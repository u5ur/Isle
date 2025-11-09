#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Passes/Pass.h>
#include <Core/Graphics/Shader/Shader.h>
#include <Core/Graphics/Texture3D/Texture3D.h>

namespace Isle
{
    class VoxelPass : public Pass
    {
    public:
        Ref<Texture3D> m_VoxelRadiance = nullptr;
        Ref<Texture3D> m_AtomicRadiance = nullptr;

        Ref<Texture3D> m_VoxelNormal = nullptr;
        Ref<Texture3D> m_AtomicNormal = nullptr;

        Ref<Texture3D> m_AtomicCounter = nullptr;

        Ref<Shader> m_MipmapShader = nullptr;
        Ref<Shader> m_BuildShader = nullptr;

        glm::ivec3 m_Resolution = glm::ivec3(256);
        glm::ivec3 m_GridMin = glm::ivec3(-20, -5, -20);
        glm::ivec3 m_GridMax = glm::ivec3(20, 10, 20);
        glm::vec3 m_CellSize = glm::vec3(0.1);
        int m_MipCount = 6;
        int m_MaxMipLevel = m_MipCount;

    public:
        virtual void Bind() override;
        virtual void Unbind() override;
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        void GenerateMipmaps();
        void BuildVoxels();
        void SetupViewport();
        void SetViewport();
        void ClearAtomics();
    };
}