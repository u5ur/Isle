#include "Mesh.h"

namespace Isle
{
    bool Mesh::GetUseViewModel()
    {
        return m_UseViewModel;
    }

    void Mesh::SetUseViewModel(bool value)
    {
        m_UseViewModel = value;
    }

    Material* Mesh::GetMaterial()
    {
        return m_Material;
    }

    void Mesh::SetMaterial(Material* material)
    {
        m_Material = material;
    }

    uint32_t Mesh::GetIndexCount()
    {
        return m_IndexCount;
    }

    void Mesh::SetIndexCount(uint32_t index)
    {
        m_IndexCount = index;
    }

    uint64_t Mesh::GetGpuOffset()
    {
        return m_GpuOffset;
    }

    void Mesh::SetGpuOffset(uint64_t offset)
    {
        m_GpuOffset = offset;
    }

    uint32_t Mesh::GetVertexOffset() const
    {
        return static_cast<uint32_t>(m_GpuOffset & 0xFFFFFFFFu);
    }

    uint32_t Mesh::GetIndexOffset() const
    {
        return static_cast<uint32_t>((m_GpuOffset >> 32) & 0xFFFFFFFFu);
    }

    void Mesh::SetVertexOffset(uint32_t vertex_offset)
    {
        uint32_t index_offset = GetIndexOffset();
        m_GpuOffset = PackGpuOffset(vertex_offset, index_offset);
    }

    void Mesh::SetIndexOffset(uint32_t index_offset)
    {
        uint32_t vertex_offset = GetVertexOffset();
        m_GpuOffset = PackGpuOffset(vertex_offset, index_offset);
    }

    uint64_t Mesh::PackGpuOffset(uint32_t vertex_offset, uint32_t index_offset)
    {
        return (static_cast<uint64_t>(index_offset) << 32) |
            static_cast<uint64_t>(vertex_offset);
    }
}
