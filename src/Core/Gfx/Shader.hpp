#pragma once
#include "../GL.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <string>

namespace Modeye::Gfx
{

class Shader
{
public:
    Shader(const std::string& vertShaderPath, const std::string& fragShaderPath);
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;
    ~Shader();

    GLuint id() const;
    void use() const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setBool(const std::string& name, bool value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat3(const std::string& name, const glm::mat3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

    bool reload();

private:
    GLuint m_id = 0;
    std::filesystem::path m_vertShaderFsPath;
    std::filesystem::path m_fragShaderFsPath;
};

}
