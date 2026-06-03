#include "../Log.hpp"
#include "Shader.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

Modeye::Gfx::Shader::Shader(const std::string& vertShader_path, const std::string& fragShaderPath)
{
    std::filesystem::path dir("shaders");
    m_vertShaderFsPath = dir / vertShader_path;
    m_fragShaderFsPath = dir / fragShaderPath;

    reload();
}

Modeye::Gfx::Shader::~Shader()
{
    glDeleteProgram(m_id);
}

GLuint Modeye::Gfx::Shader::id() const
{
    return m_id;
}

void Modeye::Gfx::Shader::use() const
{
    glUseProgram(m_id);
}

void Modeye::Gfx::Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Modeye::Gfx::Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Modeye::Gfx::Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}

void Modeye::Gfx::Shader::reload()
{
    std::string vertShaderSrc;
    std::string fragShaderSrc;

    std::ifstream vertShaderFile;
    std::ifstream fragShaderFile;

    vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vertShaderFile.open(m_vertShaderFsPath);
        fragShaderFile.open(m_fragShaderFsPath);

        std::stringstream vertShaderStream;
        std::stringstream fragShaderStream;

        vertShaderStream << vertShaderFile.rdbuf();
        fragShaderStream << fragShaderFile.rdbuf();

        vertShaderFile.close();
        fragShaderFile.close();

        vertShaderSrc = vertShaderStream.str();
        fragShaderSrc = fragShaderStream.str();
    } catch (const std::exception& e) {
        Modeye::Log::error("Failed to open shader file: {}", e.what());
        std::abort();
    }

    const char* vertShaderSrcCStr = vertShaderSrc.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSrcCStr, nullptr);
    glCompileShader(vertShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, sizeof(infoLog), nullptr, infoLog);
        Modeye::Log::error("Vertex shader compilation failed: {}", infoLog);
        std::abort();
    };

    const char* fragShaderSrcCStr = fragShaderSrc.c_str();
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSrcCStr, nullptr);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, sizeof(infoLog), nullptr, infoLog);
        Modeye::Log::error("Fragment shader compilation failed: {}", infoLog);
        std::abort();
    };

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        Modeye::Log::error("Shader program linkage failed: {}", infoLog);
        std::abort();
    }

    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    if (m_id != 0) {
        glDeleteProgram(m_id);
    }

    m_id = program;

    Modeye::Log::info("Shaders compiled and linked successfully:");
    Modeye::Log::info("  Vertex shader path: '{}'", m_vertShaderFsPath.string());
    Modeye::Log::info("  Fragment shader path: '{}'", m_fragShaderFsPath.string());
}
