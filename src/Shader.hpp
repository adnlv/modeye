#pragma once

#include <cstdlib>
#include <exception>
#include <fstream>
#include <glad/gl.h>
#include <iostream>
#include <sstream>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertShaderPath, const std::string& fragShaderPath);
    Shader(const Shader& shader) = delete;
    ~Shader();

    void use() const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    GLuint m_id;
};

Shader::Shader(const std::string& vertShaderPath, const std::string& fragShaderPath)
{
    std::string dir = "shaders";
#ifdef WIN32
    dir += '\\';
#else
    dir += '/';
#endif

    std::string vertShaderSrc;
    std::ifstream vertShaderFile;
    vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string fragShaderSrc;
    std::ifstream fragShaderFile;
    fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertShaderFile.open(dir + vertShaderPath);
        std::stringstream vertShaderStream;
        vertShaderStream << vertShaderFile.rdbuf();
        vertShaderSrc = vertShaderStream.str();
        vertShaderFile.close();

        fragShaderFile.open(dir + fragShaderPath);
        std::stringstream fragShaderStream;
        fragShaderStream << fragShaderFile.rdbuf();
        fragShaderSrc = fragShaderStream.str();
        fragShaderFile.close();
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

    m_id = program;
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
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
