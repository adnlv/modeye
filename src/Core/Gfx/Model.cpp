#include "Model.hpp"
#include "../Log.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>
#include <limits>
#include <algorithm>

// Hash specialization for Vertex deduplication
struct VertexHasher
{
    size_t operator()(const Modeye::Gfx::Vertex& v) const
    {
        size_t h1 = std::hash<float>()(v.position.x) ^ (std::hash<float>()(v.position.y) << 1) ^ (std::hash<float>()(v.position.z) << 2);
        size_t h2 = std::hash<float>()(v.normal.x) ^ (std::hash<float>()(v.normal.y) << 1) ^ (std::hash<float>()(v.normal.z) << 2);
        size_t h3 = std::hash<float>()(v.uv.x) ^ (std::hash<float>()(v.uv.y) << 1);
        size_t h4 = std::hash<float>()(v.color.r) ^ (std::hash<float>()(v.color.g) << 1) ^ (std::hash<float>()(v.color.b) << 2);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

Modeye::Gfx::Model::Model(const std::filesystem::path& filepath)
{
    loadFromFile(filepath);
}

bool Modeye::Gfx::Model::loadFromFile(const std::filesystem::path& filepath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::string baseDir = filepath.parent_path().string();
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.string().c_str(), baseDir.empty() ? nullptr : baseDir.c_str(), true);

    if (!warn.empty()) {
        Modeye::Log::warn("tinyobj warning: {}", warn);
    }
    if (!err.empty()) {
        Modeye::Log::error("tinyobj error: {}", err);
    }
    if (!ret) {
        Modeye::Log::error("Failed to load OBJ file: {}", filepath.string());
        return false;
    }

    m_name = filepath.stem().string();
    m_hasMaterials = !materials.empty();

    std::vector<Modeye::Gfx::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<Modeye::Gfx::Vertex, uint32_t, VertexHasher> uniqueVertices;

    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            size_t fv = shape.mesh.num_face_vertices[f];

            glm::vec3 matColor(1.0f);
            if (f < shape.mesh.material_ids.size()) {
                int matId = shape.mesh.material_ids[f];
                if (matId >= 0 && static_cast<size_t>(matId) < materials.size()) {
                    matColor = glm::vec3(
                        materials[matId].diffuse[0],
                        materials[matId].diffuse[1],
                        materials[matId].diffuse[2]
                    );
                }
            }

            for (size_t v = 0; v < fv; ++v) {
                const auto& index = shape.mesh.indices[index_offset + v];
                Modeye::Gfx::Vertex vertex{};
                vertex.color = matColor;

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                    minBounds = glm::min(minBounds, vertex.position);
                    maxBounds = glm::max(maxBounds, vertex.position);
                }

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                auto it = uniqueVertices.find(vertex);
                if (it == uniqueVertices.end()) {
                    uint32_t newIndex = static_cast<uint32_t>(vertices.size());
                    uniqueVertices[vertex] = newIndex;
                    vertices.push_back(vertex);
                    indices.push_back(newIndex);
                } else {
                    indices.push_back(it->second);
                }
            }
            index_offset += fv;
        }
    }

    if (!vertices.empty()) {
        m_boundsMin = minBounds;
        m_boundsMax = maxBounds;
        m_center = (minBounds + maxBounds) * 0.5f;
        m_size = maxBounds - minBounds;
    }

    // If normals were missing entirely, compute face normals per triangle
    if (attrib.normals.empty()) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            glm::vec3 v0 = vertices[indices[i + 0]].position;
            glm::vec3 v1 = vertices[indices[i + 1]].position;
            glm::vec3 v2 = vertices[indices[i + 2]].position;
            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            vertices[indices[i + 0]].normal += normal;
            vertices[indices[i + 1]].normal += normal;
            vertices[indices[i + 2]].normal += normal;
        }
        for (auto& v : vertices) {
            v.normal = glm::normalize(v.normal);
        }
    }

    m_vertexCount = vertices.size();
    m_indexCount = indices.size();
    m_mesh = std::make_unique<Modeye::Gfx::Mesh>(vertices, indices);

    Modeye::Log::info("Loaded model '{}' from '{}': {} unique vertices, {} indices (triangles: {})",
        m_name, filepath.string(), m_vertexCount, m_indexCount, m_indexCount / 3);

    return true;
}

void Modeye::Gfx::Model::draw(GLenum mode) const
{
    if (m_mesh) {
        m_mesh->draw(mode);
    }
}
