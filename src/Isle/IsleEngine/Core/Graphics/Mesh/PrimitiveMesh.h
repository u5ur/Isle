#pragma once
#include "StaticMesh.h"

namespace Isle
{
    class PrimitiveMesh : public StaticMesh
    {
    public:
        glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    public:
        void SetColor(glm::vec4 color);

    protected:
        virtual void Build() {};
    };

    class CubeMesh : public PrimitiveMesh
    {
    public:
        CubeMesh();
        virtual void Build() override;
    };

    class PlaneMesh : public PrimitiveMesh
    {
    public:
        PlaneMesh();
        virtual void Build() override;
    };

    class SphereMesh : public PrimitiveMesh
    {
    public:
        SphereMesh();
        virtual void Build() override;
    };
}