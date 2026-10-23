#include "../Log.hpp"
#include "Shader.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

namespace
{

std::string getShaderInfoLog(GLuint shader)
{
    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength <= 0) {
        return {};
    }

    std::string infoLog(logLength, '\0');
    GLsizei written = 0;
    glGetShaderInfoLog(shader, logLength, &written, infoLog.data());
    if (written >= 0 && written < logLength) {
        infoLog.resize(written);
    }

    while (!infoLog.empty() && (infoLog.back() == '\0' || infoLog.back() == '\n' || infoLog.back() == '\r')) {
        infoLog.pop_back();
    }
    return infoLog;
}

std::string getProgramInfoLog(GLuint program)
{
    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength <= 0) {
        return {};
    }

    std::string infoLog(logLength, '\0');
    GLsizei written = 0;
    glGetProgramInfoLog(program, logLength, &written, infoLog.data());
    if (written >= 0 && written < logLength) {
        infoLog.resize(written);
    }

    while (!infoLog.empty() && (infoLog.back() == '\0' || infoLog.back() == '\n' || infoLog.back() == '\r')) {
        infoLog.pop_back();
    }
    return infoLog;
}

} // namespace

Modeye::Gfx::Shader::Shader(const std::string& vertShader_path, const std::string& fragShaderPath)
{
    std::filesystem::path dir("shaders");
    m_vertShaderFsPath = dir / vertShader_path;
    m_fragShaderFsPath = dir / fragShaderPath;

    if (!reload()) {
        Modeye::Log::error("Initial shader compilation failed: '{}' and '{}'", m_vertShaderFsPath.string(), m_fragShaderFsPath.string());
        std::abort();
    }
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

void Modeye::Gfx::Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<int>(value));
}

void Modeye::Gfx::Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void Modeye::Gfx::Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}

void Modeye::Gfx::Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}

bool Modeye::Gfx::Shader::reload()
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
        return false;
    }

    const char* vertShaderSrcCStr = vertShaderSrc.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSrcCStr, nullptr);
    glCompileShader(vertShader);

    int success;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::string infoLog = getShaderInfoLog(vertShader);
        Modeye::Log::error("Vertex shader compilation failed: {}", infoLog);
        glDeleteShader(vertShader);
        return false;
    }

    const char* fragShaderSrcCStr = fragShaderSrc.c_str();
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSrcCStr, nullptr);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::string infoLog = getShaderInfoLog(fragShader);
        Modeye::Log::error("Fragment shader compilation failed: {}", infoLog);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        return false;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::string infoLog = getProgramInfoLog(program);
        Modeye::Log::error("Shader program linkage failed: {}", infoLog);
        glDetachShader(program, vertShader);
        glDetachShader(program, fragShader);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        glDeleteProgram(program);
        return false;
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
    return true;
}
