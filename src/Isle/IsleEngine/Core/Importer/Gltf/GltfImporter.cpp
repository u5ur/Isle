#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include "GltfImporter.h"
#include <thread>
#include <future>
#include <algorithm>

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
        m_BasePath = (last_slash != std::string::npos) ? file_path.substr(0, last_slash + 1) : "";
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

        m_Textures.resize(m_Model.textures.size());
        m_Materials.resize(m_Model.materials.size());
        m_SceneComponents.reserve(m_Model.nodes.size());

        LoadTextures();

        auto materialsFuture = std::async(std::launch::async, [this]() { LoadMaterials(); });
        auto meshesFuture = std::async(std::launch::async, [this]() { LoadStaticMeshes(); });

        materialsFuture.wait();
        meshesFuture.wait();

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
        SceneComponent* component = new SceneComponent();
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
            auto it = m_MeshToPrimitives.find(node.mesh);
            if (it != m_MeshToPrimitives.end())
            {
                for (int primitiveIdx : it->second)
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
        std::vector<std::vector<int>> meshPrimitives(m_Model.meshes.size());

        int totalPrimitives = 0;
        for (const auto& mesh : m_Model.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                if (primitive.mode == TINYGLTF_MODE_TRIANGLES)
                    totalPrimitives++;
            }
        }

        m_StaticMeshes.resize(totalPrimitives);

        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;

        std::vector<std::future<void>> futures;
        std::atomic<int> primitiveIndex(0);

        for (size_t meshIdx = 0; meshIdx < m_Model.meshes.size(); meshIdx++)
        {
            const tinygltf::Mesh& gltf_mesh = m_Model.meshes[meshIdx];

            for (size_t primIdx = 0; primIdx < gltf_mesh.primitives.size(); primIdx++)
            {
                const tinygltf::Primitive& primitive = gltf_mesh.primitives[primIdx];

                if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
                    continue;

                int currentPrimIdx = primitiveIndex.fetch_add(1);
                meshPrimitives[meshIdx].push_back(currentPrimIdx);

                futures.push_back(std::async(std::launch::async, [this, &gltf_mesh, &primitive, currentPrimIdx]() {

                    // Get position accessor
                    auto posIt = primitive.attributes.find("POSITION");
                    if (posIt == primitive.attributes.end())
                        return;

                    const tinygltf::Accessor& pos_accessor = m_Model.accessors[posIt->second];
                    const tinygltf::BufferView& pos_view = m_Model.bufferViews[pos_accessor.bufferView];
                    const tinygltf::Buffer& pos_buffer = m_Model.buffers[pos_view.buffer];
                    const float* positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_view.byteOffset + pos_accessor.byteOffset]);

                    // Get optional attributes
                    const float* normals = nullptr;
                    auto normIt = primitive.attributes.find("NORMAL");
                    if (normIt != primitive.attributes.end())
                    {
                        const tinygltf::Accessor& norm_accessor = m_Model.accessors[normIt->second];
                        const tinygltf::BufferView& norm_view = m_Model.bufferViews[norm_accessor.bufferView];
                        const tinygltf::Buffer& norm_buffer = m_Model.buffers[norm_view.buffer];
                        normals = reinterpret_cast<const float*>(&norm_buffer.data[norm_view.byteOffset + norm_accessor.byteOffset]);
                    }

                    const float* texcoords = nullptr;
                    auto texIt = primitive.attributes.find("TEXCOORD_0");
                    if (texIt != primitive.attributes.end())
                    {
                        const tinygltf::Accessor& tex_accessor = m_Model.accessors[texIt->second];
                        const tinygltf::BufferView& tex_view = m_Model.bufferViews[tex_accessor.bufferView];
                        const tinygltf::Buffer& tex_buffer = m_Model.buffers[tex_view.buffer];
                        texcoords = reinterpret_cast<const float*>(&tex_buffer.data[tex_view.byteOffset + tex_accessor.byteOffset]);
                    }

                    const float* tangents = nullptr;
                    auto tanIt = primitive.attributes.find("TANGENT");
                    if (tanIt != primitive.attributes.end())
                    {
                        const tinygltf::Accessor& tan_accessor = m_Model.accessors[tanIt->second];
                        const tinygltf::BufferView& tan_view = m_Model.bufferViews[tan_accessor.bufferView];
                        const tinygltf::Buffer& tan_buffer = m_Model.buffers[tan_view.buffer];
                        tangents = reinterpret_cast<const float*>(&tan_buffer.data[tan_view.byteOffset + tan_accessor.byteOffset]);
                    }

                    // Allocate vertices
                    std::vector<GpuVertex> vertices(pos_accessor.count);

                    glm::vec3 minBounds(FLT_MAX);
                    glm::vec3 maxBounds(-FLT_MAX);

                    // Build vertices in single pass
                    for (size_t v = 0; v < pos_accessor.count; v++)
                    {
                        const size_t p3 = v * 3;
                        const size_t p2 = v * 2;
                        const size_t p4 = v * 4;

                        glm::vec3 position = glm::vec3(positions[p3], positions[p3 + 1], positions[p3 + 2]);
                        glm::vec3 normal = normals ? glm::vec3(normals[p3], normals[p3 + 1], normals[p3 + 2]) : glm::vec3(0, 1, 0);
                        glm::vec2 texCoord = texcoords ? glm::vec2(texcoords[p2], 1.0f - texcoords[p2 + 1]) : glm::vec2(0, 0);
                        glm::vec3 tangent = tangents ? glm::vec3(tangents[p4], tangents[p4 + 1], tangents[p4 + 2]) : glm::vec3(1, 0, 0);
                        glm::vec4 color = glm::vec4(1.0f);

                        // Use the packed vertex constructor
                        vertices[v] = GpuVertex(position, normal, tangent, texCoord, color);

                        minBounds = glm::min(minBounds, position);
                        maxBounds = glm::max(maxBounds, position);
                    }

                    // Load indices
                    const tinygltf::Accessor& idx_accessor = m_Model.accessors[primitive.indices];
                    const tinygltf::BufferView& idx_view = m_Model.bufferViews[idx_accessor.bufferView];
                    const tinygltf::Buffer& idx_buffer = m_Model.buffers[idx_view.buffer];

                    std::vector<unsigned int> indices(idx_accessor.count);

                    if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                    {
                        const uint16_t* idx_data = reinterpret_cast<const uint16_t*>(&idx_buffer.data[idx_view.byteOffset + idx_accessor.byteOffset]);
                        for (size_t j = 0; j < idx_accessor.count; j++)
                            indices[j] = idx_data[j];
                    }
                    else if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                    {
                        const uint32_t* idx_data = reinterpret_cast<const uint32_t*>(&idx_buffer.data[idx_view.byteOffset + idx_accessor.byteOffset]);
                        memcpy(indices.data(), idx_data, idx_accessor.count * sizeof(uint32_t));
                    }
                    else if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                    {
                        const uint8_t* idx_data = reinterpret_cast<const uint8_t*>(&idx_buffer.data[idx_view.byteOffset + idx_accessor.byteOffset]);
                        for (size_t j = 0; j < idx_accessor.count; j++)
                            indices[j] = idx_data[j];
                    }

                    // Create mesh
                    StaticMesh* mesh = new StaticMesh();
                    mesh->SetVertices(std::move(vertices));
                    mesh->SetIndices(std::move(indices));
                    mesh->SetName(gltf_mesh.name);
                    mesh->m_Bounds.m_Min = minBounds;
                    mesh->m_Bounds.m_Max = maxBounds;

                    if (primitive.material >= 0)
                        mesh->SetMaterial(GetMaterial(primitive.material));
                    else
                        mesh->SetMaterial(new Material());

                    m_StaticMeshes[currentPrimIdx] = mesh;
                    }));
            }
        }

        for (auto& future : futures)
        {
            future.wait();
        }

        for (size_t i = 0; i < meshPrimitives.size(); i++)
        {
            if (!meshPrimitives[i].empty())
                m_MeshToPrimitives[i] = std::move(meshPrimitives[i]);
        }
    }

    void GltfImporter::LoadTextures()
    {
        for (size_t i = 0; i < m_Model.textures.size(); i++)
        {
            const tinygltf::Texture& gltfTex = m_Model.textures[i];
            const tinygltf::Image& image = m_Model.images[gltfTex.source];

            unsigned char* data = nullptr;
            int width, height, channels;
            TEXTURE_FORMAT format;

            if (image.uri.empty())
            {
                // Embedded image
                width = image.width;
                height = image.height;
                channels = image.component;

                format = (channels == 3) ? TEXTURE_FORMAT::RGB8 : TEXTURE_FORMAT::RGBA8;

                size_t dataSize = width * height * channels;
                data = new unsigned char[dataSize];
                memcpy(data, image.image.data(), dataSize);
            }
            else
            {
                // External image file
                std::string texturePath = m_BasePath + image.uri;
                stbi_set_flip_vertically_on_load(true);
                data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);

                if (!data)
                {
                    ISLE_ERROR("Failed to load texture: %s\n", texturePath.c_str());
                    continue;
                }

                switch (channels)
                {
                case 1: format = TEXTURE_FORMAT::R8; break;
                case 2: format = TEXTURE_FORMAT::RG8; break;
                case 3: format = TEXTURE_FORMAT::RGB8; break;
                case 4: format = TEXTURE_FORMAT::RGBA8; break;
                default: format = TEXTURE_FORMAT::RGBA8; break;
                }
            }

            // Create texture
            Texture* texture = new Texture();
            texture->Create(width, height, format, data, true);

            // Set sampler settings
            TEXTURE_FILTER minFilter = TEXTURE_FILTER::LINEAR;
            TEXTURE_FILTER magFilter = TEXTURE_FILTER::LINEAR;
            TEXTURE_WRAP wrapS = TEXTURE_WRAP::REPEAT;
            TEXTURE_WRAP wrapT = TEXTURE_WRAP::REPEAT;

            if (gltfTex.sampler >= 0)
            {
                const tinygltf::Sampler& sampler = m_Model.samplers[gltfTex.sampler];

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
            }

            texture->SetMinFilter(minFilter);
            texture->SetMagFilter(magFilter);
            texture->SetWrapS(wrapS);
            texture->SetWrapT(wrapT);
            texture->SetDebugLabel(gltfTex.name.empty() ? ("Texture_" + std::to_string(i)) : gltfTex.name);

            m_Textures[i] = texture;

            // Free image data
            if (!image.uri.empty())
                stbi_image_free(data);
            else
                delete[] data;
        }
    }

    void GltfImporter::LoadMaterials()
    {
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

            const auto& bcf = gltf_mat.pbrMetallicRoughness.baseColorFactor;
            material->SetBaseColorFactor(glm::vec4(bcf[0], bcf[1], bcf[2], bcf[3]));

            if (material->GetBaseColorFactor().a < 0.999f)
            {
                material->SetTransparent(true);
            }

            material->SetMetallicFactor(gltf_mat.pbrMetallicRoughness.metallicFactor == 1.0 ? 0.0f : static_cast<float>(gltf_mat.pbrMetallicRoughness.metallicFactor));
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

            const auto& ef = gltf_mat.emissiveFactor;
            material->SetEmissiveFactor(glm::vec3(ef[0], ef[1], ef[2]));
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