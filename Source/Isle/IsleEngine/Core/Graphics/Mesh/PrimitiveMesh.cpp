#include "PrimitiveMesh.h"
#include <Core/Graphics/Material/Material.h>

namespace Isle
{
    void PrimitiveMesh::SetColor(glm::vec4 color)
    {
        m_Color = color;
        Build();
    }

    CubeMesh::CubeMesh()
    {
        Build();
        m_Material = New<Material>();
        SetName("Cube");
    }

    void CubeMesh::Build()
    {
        auto makeVertex = [this](const glm::vec3& pos, const glm::vec3& normal,
            const glm::vec2& uv, const glm::vec3& tangent) {
                return GpuVertex(pos, normal, tangent, uv, m_Color);
            };

        std::vector<Isle::GpuVertex> Vertices =
        {
            makeVertex({-0.5f,-0.5f, 0.5f}, {0,0,1}, {0,0}, {1,0,0}),
            makeVertex({ 0.5f,-0.5f, 0.5f}, {0,0,1}, {1,0}, {1,0,0}),
            makeVertex({ 0.5f, 0.5f, 0.5f}, {0,0,1}, {1,1}, {1,0,0}),
            makeVertex({-0.5f, 0.5f, 0.5f}, {0,0,1}, {0,1}, {1,0,0}),

            makeVertex({ 0.5f,-0.5f,-0.5f}, {0,0,-1}, {0,0}, {-1,0,0}),
            makeVertex({-0.5f,-0.5f,-0.5f}, {0,0,-1}, {1,0}, {-1,0,0}),
            makeVertex({-0.5f, 0.5f,-0.5f}, {0,0,-1}, {1,1}, {-1,0,0}),
            makeVertex({ 0.5f, 0.5f,-0.5f}, {0,0,-1}, {0,1}, {-1,0,0}),

            makeVertex({-0.5f, 0.5f, 0.5f}, {0,1,0}, {0,0}, {1,0,0}),
            makeVertex({ 0.5f, 0.5f, 0.5f}, {0,1,0}, {1,0}, {1,0,0}),
            makeVertex({ 0.5f, 0.5f,-0.5f}, {0,1,0}, {1,1}, {1,0,0}),
            makeVertex({-0.5f, 0.5f,-0.5f}, {0,1,0}, {0,1}, {1,0,0}),

            makeVertex({-0.5f,-0.5f,-0.5f}, {0,-1,0}, {0,0}, {1,0,0}),
            makeVertex({ 0.5f,-0.5f,-0.5f}, {0,-1,0}, {1,0}, {1,0,0}),
            makeVertex({ 0.5f,-0.5f, 0.5f}, {0,-1,0}, {1,1}, {1,0,0}),
            makeVertex({-0.5f,-0.5f, 0.5f}, {0,-1,0}, {0,1}, {1,0,0}),

            makeVertex({ 0.5f,-0.5f, 0.5f}, {1,0,0}, {0,0}, {0,1,0}),
            makeVertex({ 0.5f,-0.5f,-0.5f}, {1,0,0}, {1,0}, {0,1,0}),
            makeVertex({ 0.5f, 0.5f,-0.5f}, {1,0,0}, {1,1}, {0,1,0}),
            makeVertex({ 0.5f, 0.5f, 0.5f}, {1,0,0}, {0,1}, {0,1,0}),

            makeVertex({-0.5f,-0.5f,-0.5f}, {-1,0,0}, {0,0}, {0,1,0}),
            makeVertex({-0.5f,-0.5f, 0.5f}, {-1,0,0}, {1,0}, {0,1,0}),
            makeVertex({-0.5f, 0.5f, 0.5f}, {-1,0,0}, {1,1}, {0,1,0}),
            makeVertex({-0.5f, 0.5f,-0.5f}, {-1,0,0}, {0,1}, {0,1,0})
        };

        std::vector<unsigned int> Indices =
        {
            0,1,2, 2,3,0,
            4,5,6, 6,7,4,
            8,9,10, 10,11,8,
            12,13,14, 14,15,12,
            16,17,18, 18,19,16,
            20,21,22, 22,23,20
        };

        m_Bounds.m_Min = glm::vec3(FLT_MAX);
        m_Bounds.m_Max = glm::vec3(-FLT_MAX);
        for (auto& v : Vertices)
        {
            m_Bounds.m_Min = glm::min(m_Bounds.m_Min, v.m_Position);
            m_Bounds.m_Max = glm::max(m_Bounds.m_Max, v.m_Position);
        }

        m_Vertices = Vertices;
        m_Indices = Indices;
    }

    PlaneMesh::PlaneMesh()
    {
        Build();
        m_Material = New<Material>();
    }

    void PlaneMesh::Build()
    {
        auto makeVertex = [this](const glm::vec3& pos, const glm::vec3& normal,
            const glm::vec2& uv, const glm::vec3& tangent) {
                return GpuVertex(pos, normal, tangent, uv, m_Color);
            };

        std::vector<Isle::GpuVertex> Vertices =
        {
            makeVertex({-0.5f, 0.0f,  0.5f}, {0,1,0}, {0,0}, {1,0,0}),
            makeVertex({ 0.5f, 0.0f,  0.5f}, {0,1,0}, {1,0}, {1,0,0}),
            makeVertex({ 0.5f, 0.0f, -0.5f}, {0,1,0}, {1,1}, {1,0,0}),
            makeVertex({-0.5f, 0.0f, -0.5f}, {0,1,0}, {0,1}, {1,0,0})
        };

        std::vector<unsigned int> Indices = { 0, 1, 2, 2, 3, 0 };

        m_Bounds.m_Min = glm::vec3(FLT_MAX);
        m_Bounds.m_Max = glm::vec3(-FLT_MAX);
        for (auto& v : Vertices)
        {
            m_Bounds.m_Min = glm::min(m_Bounds.m_Min, v.m_Position);
            m_Bounds.m_Max = glm::max(m_Bounds.m_Max, v.m_Position);
        }

        m_Vertices = Vertices;
        m_Indices = Indices;
    }

    SphereMesh::SphereMesh()
    {
        Build();
        m_Material = New<Material>();
    }

    void SphereMesh::Build()
    {
        std::vector<Isle::GpuVertex> Vertices;
        std::vector<unsigned int> Indices;

        const int segments = 32;
        const int rings = 16;
        const float radius = 0.5f;

        for (int ring = 0; ring <= rings; ++ring)
        {
            float theta = ring * glm::pi<float>() / rings;
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            for (int segment = 0; segment <= segments; ++segment)
            {
                float phi = segment * 2.0f * glm::pi<float>() / segments;
                float sinPhi = std::sin(phi);
                float cosPhi = std::cos(phi);

                glm::vec3 position;
                position.x = radius * sinTheta * cosPhi;
                position.y = radius * cosTheta;
                position.z = radius * sinTheta * sinPhi;

                glm::vec3 normal = glm::normalize(position);

                glm::vec2 texCoord;
                texCoord.x = 1.0f - (float)segment / segments;
                texCoord.y = 1.0f - (float)ring / rings;

                glm::vec3 tangent = glm::normalize(glm::vec3(
                    -radius * sinTheta * sinPhi, 0.0f, radius * sinTheta * cosPhi));

                Vertices.emplace_back(position, normal, tangent, texCoord, m_Color);
            }
        }

        for (int ring = 0; ring < rings; ++ring)
        {
            for (int segment = 0; segment < segments; ++segment)
            {
                int first = ring * (segments + 1) + segment;
                int second = first + segments + 1;
                Indices.push_back(first);
                Indices.push_back(first + 1);
                Indices.push_back(second);
                Indices.push_back(first + 1);
                Indices.push_back(second + 1);
                Indices.push_back(second);
            }
        }

        m_Bounds.m_Min = glm::vec3(FLT_MAX);
        m_Bounds.m_Max = glm::vec3(-FLT_MAX);
        for (auto& v : Vertices)
        {
            m_Bounds.m_Min = glm::min(m_Bounds.m_Min, v.m_Position);
            m_Bounds.m_Max = glm::max(m_Bounds.m_Max, v.m_Position);
        }

        m_Vertices = Vertices;
        m_Indices = Indices;
    }
}