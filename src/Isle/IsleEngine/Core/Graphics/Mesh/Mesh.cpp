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
        return m_Material.Get();
    }

    void Mesh::SetMaterial(Material* material)
    {
        m_Material = Ref<Material>(material);
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
        m_Vertices = vertices;
    }

    void Mesh::SetIndices(std::vector<unsigned int> indices)
    {
        m_Indices = indices;
    }
}
