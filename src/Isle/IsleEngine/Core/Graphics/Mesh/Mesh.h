#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Material/Material.h>

namespace Isle
{
    class Mesh : public SceneComponent
    {
    protected:
        Ref<Material> m_Material = nullptr;
        bool m_UseViewModel = false;
        std::vector<GpuVertex> m_Vertices;
        std::vector<unsigned int> m_Indices;


    public:
        bool GetUseViewModel();
        void SetUseViewModel(bool value);

        Material* GetMaterial();

        const std::vector<GpuVertex>& GetVertices() const { return m_Vertices; }
        const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    };
}
