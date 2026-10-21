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

private:
    std::string m_name;
    std::unique_ptr<Mesh> m_mesh;
    size_t m_vertexCount = 0;
    size_t m_indexCount = 0;
};

}
