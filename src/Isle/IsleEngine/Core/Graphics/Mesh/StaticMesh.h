#pragma once
#include "Mesh.h"

namespace Isle
{
    class StaticMesh : public Mesh
    {
    public:
        GpuStaticMesh GetGpuStaticMesh();
    };
}

