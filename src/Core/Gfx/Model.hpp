#pragma once
#include "Mesh.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace Modeye::Gfx
{

class Model
{
public:
    Model() = default;
    explicit Model(const std::filesystem::path& filepath);
    ~Model() = default;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) noexcept = default;
    Model& operator=(Model&&) noexcept = default;

    bool loadFromFile(const std::filesystem::path& filepath);
    void draw(GLenum mode = GL_TRIANGLES) const;

    bool isValid() const { return m_mesh != nullptr; }
    const std::string& name() const { return m_name; }
    size_t vertexCount() const { return m_vertexCount; }
    size_t indexCount() const { return m_indexCount; }
    bool hasMaterials() const { return m_hasMaterials; }
    const glm::vec3& center() const { return m_center; }
    const glm::vec3& boundsMin() const { return m_boundsMin; }
    const glm::vec3& boundsMax() const { return m_boundsMax; }
    const glm::vec3& size() const { return m_size; }

private:
    std::string m_name;
    std::unique_ptr<Mesh> m_mesh;
    size_t m_vertexCount = 0;
    size_t m_indexCount = 0;
    bool m_hasMaterials = false;
    glm::vec3 m_boundsMin{ 0.0f };
    glm::vec3 m_boundsMax{ 0.0f };
    glm::vec3 m_center{ 0.0f };
    glm::vec3 m_size{ 0.0f };
};

}
