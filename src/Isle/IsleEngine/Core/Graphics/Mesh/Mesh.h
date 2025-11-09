#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Material/Material.h>

namespace Isle
{
    class Mesh : public SceneComponent
    {
    public:
        int m_Id = -1;

    protected:
        Ref<Material> m_Material = nullptr;
        bool m_UseViewModel = false;
        std::vector<GpuVertex> m_Vertices;
        std::vector<unsigned int> m_Indices;

    public:
        bool GetUseViewModel();
        void SetUseViewModel(bool value);

        Material* GetMaterial();
        void SetMaterial(Material* material);

        void SetVertices(std::vector<GpuVertex> vertices);
        void SetIndices(std::vector<unsigned int> indices);

        std::vector<GpuVertex> GetVertices();
        std::vector<unsigned int> GetIndices();
    };
}
