#pragma once

#include "Log.hpp"
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

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
    void setMat4(const std::string& name, const glm::mat4& value) const;

    void reload();

private:
    GLuint m_id = 0;
    std::filesystem::path m_vertShaderFSPath;
    std::filesystem::path m_fragShaderFSPath;
};
