#include "StaticMesh.h"

namespace Isle
{
	GpuStaticMesh StaticMesh::GetGpuStaticMesh()
	{
        GpuStaticMesh GStaticMesh{};
        GStaticMesh.m_Transform = GetWorldMatrix();
        GStaticMesh.m_NormalMatrix = glm::transpose(glm::inverse(GStaticMesh.m_Transform));
        GStaticMesh.m_IndexCount = GetIndexCount();
        GStaticMesh.m_MaterialIndex = GetMaterial()->m_Version;
        GStaticMesh.m_VertexOffset = GetVertexOffset();
        GStaticMesh.m_IndexOffset = GetIndexOffset();
	}
}