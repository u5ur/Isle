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
}
