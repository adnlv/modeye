#pragma once

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <glad/gl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

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

Shader::Shader(const std::string& vertShaderPath, const std::string& fragShaderPath)
{
    std::filesystem::path dir("shaders");
    m_vertShaderFSPath = dir / vertShaderPath;
    m_fragShaderFSPath = dir / fragShaderPath;

    reload();
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

inline GLuint Shader::id() const
{
    return m_id;
}

void Shader::use() const
{
    glUseProgram(m_id);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

inline void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}

inline void Shader::reload()
{
    std::string vertShaderSrc;
    std::string fragShaderSrc;

    std::ifstream vertShaderFile;
    std::ifstream fragShaderFile;

    vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertShaderFile.open(m_vertShaderFSPath);
        fragShaderFile.open(m_fragShaderFSPath);

        std::stringstream vertShaderStream;
        std::stringstream fragShaderStream;

        vertShaderStream << vertShaderFile.rdbuf();
        fragShaderStream << fragShaderFile.rdbuf();

        vertShaderFile.close();
        fragShaderFile.close();

        vertShaderSrc = vertShaderStream.str();
        fragShaderSrc = fragShaderStream.str();
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ >> " << e.what() << std::endl;
        std::abort();
    }

    const char* vertShaderSrcCStr = vertShaderSrc.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSrcCStr, nullptr);
    glCompileShader(vertShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertShader, sizeof(infoLog), nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED >> " << infoLog << std::endl;
        std::abort();
    };

    const char* fragShaderSrcCStr = fragShaderSrc.c_str();
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSrcCStr, nullptr);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragShader, sizeof(infoLog), nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED >> " << infoLog << std::endl;
        std::abort();
    };

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED >> " << infoLog << std::endl;
        std::abort();
    }

    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    if (m_id != 0)
    {
        glDeleteProgram(m_id);
    }

    m_id = program;
}
