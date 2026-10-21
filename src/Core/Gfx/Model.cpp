#include "Model.hpp"
#include "../Log.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>

// Hash specialization for Vertex deduplication
struct VertexHasher
{
    size_t operator()(const Modeye::Gfx::Vertex& v) const
    {
        size_t h1 = std::hash<float>()(v.position.x) ^ (std::hash<float>()(v.position.y) << 1) ^ (std::hash<float>()(v.position.z) << 2);
        size_t h2 = std::hash<float>()(v.normal.x) ^ (std::hash<float>()(v.normal.y) << 1) ^ (std::hash<float>()(v.normal.z) << 2);
        size_t h3 = std::hash<float>()(v.uv.x) ^ (std::hash<float>()(v.uv.y) << 1);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
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

    std::vector<Modeye::Gfx::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<Modeye::Gfx::Vertex, uint32_t, VertexHasher> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Modeye::Gfx::Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
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
