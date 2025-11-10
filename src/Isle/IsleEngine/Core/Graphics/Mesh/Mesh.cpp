#include "Mesh.h"

namespace Isle
{
    Mesh::Mesh()
    {
    }

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
        return m_Material.Get();
    }

    void Mesh::SetMaterial(Material* material)
    {
        if (m_Material.Get() != material)
        {
            m_Material = Ref<Material>(material);
        }
    }

    std::vector<GpuVertex> Mesh::GetVertices()
    {
        return m_Vertices;
    }

    std::vector<unsigned int> Mesh::GetIndices()
    {
        return m_Indices;
    }

    void Mesh::SetVertices(std::vector<GpuVertex> vertices)
    {
        m_Vertices = std::move(vertices);
        MarkDirty();
    }

    void Mesh::SetIndices(std::vector<unsigned int> indices)
    {
        m_Indices = std::move(indices);
        MarkDirty();
    }

    bool Mesh::IsDirty()
    {
        return m_Dirty || IsTransformDirty();
    }

    void Mesh::MarkDirty(bool value)
    {
        m_Dirty = value;
        m_TransformDirty = value;
    }
}
