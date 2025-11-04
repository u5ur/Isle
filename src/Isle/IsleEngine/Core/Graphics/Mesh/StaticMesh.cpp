#include "StaticMesh.h"
#include <Core/Graphics/Render.h>
#include <Core/Graphics/Pipeline/Pipeline.h>

namespace Isle
{
	GpuStaticMesh StaticMesh::GetGpuStaticMesh()
	{
        GpuStaticMesh GStaticMesh{};
        GStaticMesh.m_MaterialIndex = Render::Instance()->GetPipeline()->GetNumMaterials();
        GStaticMesh.m_Transform = GetWorldMatrix();
        GStaticMesh.m_NormalMatrix = glm::transpose(glm::inverse(GStaticMesh.m_Transform));
        GStaticMesh.m_VertexOffset = Render::Instance()->GetPipeline()->GetNumVertices();
        GStaticMesh.m_IndexOffset = Render::Instance()->GetPipeline()->GetNumIndicies();
        GStaticMesh.m_IndexCount = GetIndices().size();
        GStaticMesh.m_UseViewModel = 0;
        return GStaticMesh;
	}
}