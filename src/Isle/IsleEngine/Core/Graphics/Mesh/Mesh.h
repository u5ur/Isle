#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Material/Material.h>

namespace Isle
{
    class Mesh : public SceneComponent
    {
    protected:
        Material* m_Material = nullptr;
        bool m_UseViewModel = false;
        uint64_t m_GpuOffset = 0;
        uint32_t m_IndexCount = 0;
        std::vector<GpuVertex> m_Vertices;
        std::vector<unsigned int> m_Indices;


    public:
        bool GetUseViewModel();
        void SetUseViewModel(bool value);

        Material* GetMaterial();
        void SetMaterial(Material* material);

        uint32_t GetIndexCount();
        void SetIndexCount(uint32_t index);

        uint64_t GetGpuOffset();
        void SetGpuOffset(uint64_t offset);

        uint32_t GetVertexOffset() const;
        uint32_t GetIndexOffset() const;
        void SetVertexOffset(uint32_t vertex_offset);
        void SetIndexOffset(uint32_t index_offset);

        const std::vector<GpuVertex>& GetVertices() const { return m_Vertices; }
        const std::vector<unsigned int>& GetIndices() const { return m_Indices; }

        static uint64_t PackGpuOffset(uint32_t vertex_offset, uint32_t index_offset);
    };
}
