#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>


#include "GltfImporter.h"

namespace Isle
{
    tinygltf::Model m_Model;

    GltfImporter::GltfImporter()
    {
        m_RootComponent = new SceneComponent();
    }

    GltfImporter::~GltfImporter()
    {
        m_SceneComponents.clear();
        m_RootComponent = nullptr;
    }

    void GltfImporter::ExtractBasePath(const std::string& file_path)
    {
        size_t last_slash = file_path.find_last_of("/\\");
        if (last_slash != std::string::npos)
            m_BasePath = file_path.substr(0, last_slash + 1);
        else
            m_BasePath = "";
    }

    bool GltfImporter::LoadFromFile(const std::string& file_path)
    {
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        ExtractBasePath(file_path);

        bool ret = false;
        if (file_path.substr(file_path.find_last_of(".") + 1) == "glb")
            ret = loader.LoadBinaryFromFile(&m_Model, &err, &warn, file_path);
        else
            ret = loader.LoadASCIIFromFile(&m_Model, &err, &warn, file_path);

        if (!warn.empty())
            ISLE_WARN("GLTF Warning: %s\n", warn);

        if (!err.empty())
            ISLE_WARN("GLTF Error: %s\n", err);

        if (!ret)
        {
            ISLE_ERROR("Failed to load GLTF file: %s\n", file_path);
            return false;
        }

        LoadTextures();
        LoadMaterials();
        LoadStaticMeshes();

        const tinygltf::Scene& scene = m_Model.scenes[m_Model.defaultScene >= 0 ? m_Model.defaultScene : 0];
        for (int node_index : scene.nodes)
        {
            ProcessNode(node_index, nullptr);
        }

        return true;
    }

    void GltfImporter::ProcessNode(int node_index, SceneComponent* parent)
    {
        if (node_index < 0 || node_index >= m_Model.nodes.size())
            return;

        const tinygltf::Node& node = m_Model.nodes[node_index];

        SceneComponent* component = nullptr;
        
        component = new SceneComponent();
        component->SetName(node.name);
        m_SceneComponents.push_back(component);

        if (node.matrix.size() == 16)
        {
            glm::mat4 matrix;
            for (int i = 0; i < 16; i++)
                matrix[i / 4][i % 4] = static_cast<float>(node.matrix[i]);
            component->SetWorldMatrix(matrix);
        }
        else
        {
            if (node.translation.size() == 3)
            {
                component->SetLocalPosition(glm::vec3(
                    node.translation[0],
                    node.translation[1],
                    node.translation[2]
                ));
            }

            if (node.rotation.size() == 4)
            {
                component->SetLocalRotation(glm::quat(
                    static_cast<float>(node.rotation[3]),
                    static_cast<float>(node.rotation[0]),
                    static_cast<float>(node.rotation[1]),
                    static_cast<float>(node.rotation[2])
                ));
            }

            if (node.scale.size() == 3)
            {
                component->SetLocalScale(glm::vec3(
                    node.scale[0],
                    node.scale[1],
                    node.scale[2]
                ));
            }
        }

        m_NodeMap[node_index] = component;

        if (node.mesh >= 0)
        {
            const tinygltf::Mesh& gltf_mesh = m_Model.meshes[node.mesh];

            if (m_MeshToPrimitives.find(node.mesh) != m_MeshToPrimitives.end())
            {
                for (int primitiveIdx : m_MeshToPrimitives[node.mesh])
                {
                    if (primitiveIdx < m_StaticMeshes.size())
                    {
                        StaticMesh* originalMesh = m_StaticMeshes[primitiveIdx];

                        StaticMesh* instancedMesh = new StaticMesh();
                        instancedMesh->SetVertices(originalMesh->GetVertices());
                        instancedMesh->SetIndices(originalMesh->GetIndices());
                        instancedMesh->SetMaterial(originalMesh->GetMaterial());
                        instancedMesh->SetName(originalMesh->GetName());
                        instancedMesh->SetBounds(originalMesh->GetBounds());
                        component->AddChild(instancedMesh);
                    }
                }
            }
        }

        if (parent)
            parent->AddChild(component);
        else
            m_RootComponent->AddChild(component);

        for (int child_index : node.children)
        {
            ProcessNode(child_index, component);
        }
    }

    void GltfImporter::LoadStaticMeshes()
    {
        int primitiveIndex = 0;
        for (size_t i = 0; i < m_Model.meshes.size(); i++)
        {
            const tinygltf::Mesh& gltf_mesh = m_Model.meshes[i];
            std::vector<int> primitives;

            for (const tinygltf::Primitive& primitive : gltf_mesh.primitives)
            {
                if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
                    continue;
                std::vector<GpuVertex> vertices;
                std::vector<unsigned int> indices;

                const tinygltf::Accessor& pos_accessor = m_Model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& pos_view = m_Model.bufferViews[pos_accessor.bufferView];
                const tinygltf::Buffer& pos_buffer = m_Model.buffers[pos_view.buffer];
                const float* positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_view.byteOffset + pos_accessor.byteOffset]);

                const float* normals = nullptr;
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& norm_accessor = m_Model.accessors[primitive.attributes.at("NORMAL")];
                    const tinygltf::BufferView& norm_view = m_Model.bufferViews[norm_accessor.bufferView];
                    const tinygltf::Buffer& norm_buffer = m_Model.buffers[norm_view.buffer];
                    normals = reinterpret_cast<const float*>(&norm_buffer.data[norm_view.byteOffset + norm_accessor.byteOffset]);
                }

                const float* texcoords = nullptr;
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& tex_accessor = m_Model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    const tinygltf::BufferView& tex_view = m_Model.bufferViews[tex_accessor.bufferView];
                    const tinygltf::Buffer& tex_buffer = m_Model.buffers[tex_view.buffer];
                    texcoords = reinterpret_cast<const float*>(&tex_buffer.data[tex_view.byteOffset + tex_accessor.byteOffset]);
                }

                const float* tangents = nullptr;
                if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& tan_accessor = m_Model.accessors[primitive.attributes.at("TANGENT")];
                    const tinygltf::BufferView& tan_view = m_Model.bufferViews[tan_accessor.bufferView];
                    const tinygltf::Buffer& tan_buffer = m_Model.buffers[tan_view.buffer];
                    tangents = reinterpret_cast<const float*>(&tan_buffer.data[tan_view.byteOffset + tan_accessor.byteOffset]);
                }

                for (size_t v = 0; v < pos_accessor.count; v++)
                {
                    GpuVertex vertex;
                    vertex.m_Position = glm::vec3(positions[v * 3 + 0], positions[v * 3 + 1], positions[v * 3 + 2]);

                    if (normals)
                        vertex.m_Normal = glm::vec3(normals[v * 3 + 0], normals[v * 3 + 1], normals[v * 3 + 2]);
                    else
                        vertex.m_Normal = glm::vec3(0, 1, 0);

                    if (texcoords)
                        vertex.m_TexCoord = glm::vec2(texcoords[v * 2 + 0], 1.0f - texcoords[v * 2 + 1]);
                    else
                        vertex.m_TexCoord = glm::vec2(0, 0);

                    if (tangents)
                    {
                        vertex.m_Tangent = glm::vec3(tangents[v * 4 + 0], tangents[v * 4 + 1], tangents[v * 4 + 2]);
                        vertex.m_BitTangent = glm::cross(vertex.m_Normal, vertex.m_Tangent) * tangents[v * 4 + 3];
                    }
                    else
                    {
                        vertex.m_Tangent = glm::vec3(1, 0, 0);
                        vertex.m_BitTangent = glm::vec3(0, 0, 1);
                    }

                    vertex.m_Color = glm::vec4(1.0f);
                    vertices.push_back(vertex);
                }

                glm::vec3 minBounds(FLT_MAX);
                glm::vec3 maxBounds(-FLT_MAX);

                for (const auto& v : vertices)
                {
                    minBounds = glm::min(minBounds, v.m_Position);
                    maxBounds = glm::max(maxBounds, v.m_Position);
                }

                const tinygltf::Accessor& idx_accessor = m_Model.accessors[primitive.indices];
                const tinygltf::BufferView& idx_view = m_Model.bufferViews[idx_accessor.bufferView];
                const tinygltf::Buffer& idx_buffer = m_Model.buffers[idx_view.buffer];

                if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    const uint16_t* idx_data = reinterpret_cast<const uint16_t*>(&idx_buffer.data[idx_view.byteOffset + idx_accessor.byteOffset]);
                    for (size_t j = 0; j < idx_accessor.count; j++)
                        indices.push_back(static_cast<unsigned int>(idx_data[j]));
                }
                else if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    const uint32_t* idx_data = reinterpret_cast<const uint32_t*>(&idx_buffer.data[idx_view.byteOffset + idx_accessor.byteOffset]);
                    for (size_t j = 0; j < idx_accessor.count; j++)
                        indices.push_back(idx_data[j]);
                }

                StaticMesh* mesh = new StaticMesh();
                mesh->SetVertices(vertices);
                mesh->SetIndices(indices);
                mesh->SetName(gltf_mesh.name);
                mesh->m_Bounds.m_Min = minBounds;
                mesh->m_Bounds.m_Max = maxBounds;

                if (primitive.material >= 0)
                    mesh->SetMaterial(GetMaterial(primitive.material));
                else
                    mesh->SetMaterial(new Material());


                m_StaticMeshes.push_back(mesh);
                primitives.push_back(primitiveIndex);
                primitiveIndex++;
            }

            m_MeshToPrimitives[i] = primitives;
        }
    }

    void GltfImporter::LoadTextures()
    {
        m_Textures.resize(m_Model.textures.size());

        for (size_t i = 0; i < m_Model.textures.size(); i++)
        {
            const tinygltf::Texture& gltfTex = m_Model.textures[i];
            const tinygltf::Image& image = m_Model.images[gltfTex.source];

            auto texture = new Texture();

            if (image.uri.empty())
            {
                TEXTURE_FORMAT texFormat = TEXTURE_FORMAT::RGBA8;
                if (image.component == 3)
                    texFormat = TEXTURE_FORMAT::RGB8;
                else if (image.component == 4)
                    texFormat = TEXTURE_FORMAT::RGBA8;

                texture->Create(
                    image.width,
                    image.height,
                    texFormat,
                    image.image.data(),
                    true
                );
            }
            else
            {
                std::string texturePath = m_BasePath + image.uri;
                texture->CreateFromFile(texturePath, true);
            }

            if (gltfTex.sampler >= 0)
            {
                const tinygltf::Sampler& sampler = m_Model.samplers[gltfTex.sampler];

                TEXTURE_FILTER minFilter = TEXTURE_FILTER::LINEAR;
                TEXTURE_FILTER magFilter = TEXTURE_FILTER::LINEAR;
                TEXTURE_WRAP wrapS = TEXTURE_WRAP::REPEAT;
                TEXTURE_WRAP wrapT = TEXTURE_WRAP::REPEAT;

                switch (sampler.minFilter)
                {
                case TINYGLTF_TEXTURE_FILTER_NEAREST:                   minFilter = TEXTURE_FILTER::NEAREST; break;
                case TINYGLTF_TEXTURE_FILTER_LINEAR:                    minFilter = TEXTURE_FILTER::LINEAR; break;
                case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:    minFilter = TEXTURE_FILTER::NEAREST_MIPMAP_NEAREST; break;
                case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:     minFilter = TEXTURE_FILTER::LINEAR_MIPMAP_NEAREST; break;
                case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:     minFilter = TEXTURE_FILTER::NEAREST_MIPMAP_LINEAR; break;
                case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:      minFilter = TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR; break;
                }

                switch (sampler.magFilter)
                {
                case TINYGLTF_TEXTURE_FILTER_NEAREST: magFilter = TEXTURE_FILTER::NEAREST; break;
                case TINYGLTF_TEXTURE_FILTER_LINEAR:  magFilter = TEXTURE_FILTER::LINEAR; break;
                }

                texture->SetMinFilter(minFilter);
                texture->SetMagFilter(magFilter);

                switch (sampler.wrapS)
                {
                case TINYGLTF_TEXTURE_WRAP_REPEAT:          wrapS = TEXTURE_WRAP::REPEAT; break;
                case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:   wrapS = TEXTURE_WRAP::CLAMP_TO_EDGE; break;
                case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: wrapS = TEXTURE_WRAP::MIRRORED_REPEAT; break;
                }

                switch (sampler.wrapT)
                {
                case TINYGLTF_TEXTURE_WRAP_REPEAT:          wrapT = TEXTURE_WRAP::REPEAT; break;
                case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:   wrapT = TEXTURE_WRAP::CLAMP_TO_EDGE; break;
                case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: wrapT = TEXTURE_WRAP::MIRRORED_REPEAT; break;
                }

                texture->SetWrapS(wrapS);
                texture->SetWrapT(wrapT);
            }

            texture->SetDebugLabel(gltfTex.name.empty() ? ("Texture_" + std::to_string(i)) : gltfTex.name);
            m_Textures[i] = texture;
        }
    }


    void GltfImporter::LoadMaterials()
    {
        m_Materials.resize(m_Model.materials.size());

        for (size_t i = 0; i < m_Model.materials.size(); i++)
        {
            const tinygltf::Material& gltf_mat = m_Model.materials[i];
            Material* material = new Material();

            if (gltf_mat.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
                material->SetTexture("BaseColor",
                    GetTexture(gltf_mat.pbrMetallicRoughness.baseColorTexture.index));
            }

            if (gltf_mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
            {
                material->SetTexture("MetallicRoughness",
                    GetTexture(gltf_mat.pbrMetallicRoughness.metallicRoughnessTexture.index));
            }

            material->SetBaseColorFactor(glm::vec4(
                gltf_mat.pbrMetallicRoughness.baseColorFactor[0],
                gltf_mat.pbrMetallicRoughness.baseColorFactor[1],
                gltf_mat.pbrMetallicRoughness.baseColorFactor[2],
                gltf_mat.pbrMetallicRoughness.baseColorFactor[3]
            ));

            if (material->GetBaseColorFactor().a < 0.999f)
            {
                material->SetTransparent(true);
            }


            if (gltf_mat.pbrMetallicRoughness.metallicFactor == 1.0f)
            {
                material->SetMetallicFactor(static_cast<float>(0.0f));
            }
            else
            {
                material->SetMetallicFactor(static_cast<float>(gltf_mat.pbrMetallicRoughness.metallicFactor));
            }


            material->SetRoughnessFactor(static_cast<float>(gltf_mat.pbrMetallicRoughness.roughnessFactor));

            if (gltf_mat.normalTexture.index >= 0)
            {
                material->SetTexture("Normal", GetTexture(gltf_mat.normalTexture.index));
                material->SetNormalScale(static_cast<float>(gltf_mat.normalTexture.scale));
            }

            if (gltf_mat.occlusionTexture.index >= 0)
            {
                material->SetTexture("Occlusion", GetTexture(gltf_mat.occlusionTexture.index));
                material->SetOcclusionStrength(static_cast<float>(gltf_mat.occlusionTexture.strength));
            }

            if (gltf_mat.emissiveTexture.index >= 0)
            {
                material->SetTexture("Emissive", GetTexture(gltf_mat.emissiveTexture.index));
            }

            material->SetEmissiveFactor(glm::vec3(
                gltf_mat.emissiveFactor[0],
                gltf_mat.emissiveFactor[1],
                gltf_mat.emissiveFactor[2]
            ));

            material->SetName(gltf_mat.name);
            m_Materials[i] = material;
        }
    }

    StaticMesh* GltfImporter::GetStaticMesh(int mesh_index)
    {
        if (mesh_index >= 0 && mesh_index < m_StaticMeshes.size())
            return m_StaticMeshes[mesh_index];
        return nullptr;
    }

    Material* GltfImporter::GetMaterial(int material_index)
    {
        if (material_index >= 0 && material_index < m_Materials.size())
            return m_Materials[material_index];
        return nullptr;
    }

    Texture* GltfImporter::GetTexture(int texture_index)
    {
        if (texture_index >= 0 && texture_index < m_Textures.size())
            return m_Textures[texture_index];
        return nullptr;
    }
}