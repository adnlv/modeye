#pragma once
#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

namespace modeye
{
    namespace gfx
    {
        class shader
        {
        public:
            shader(const std::string& vert_shader_path, const std::string& frag_shader_path);
            shader(const shader& other) = delete;
            shader& operator=(const shader& other) = delete;
            ~shader();

            GLuint id() const;
            void use() const;
            void set_int(const std::string& name, int value) const;
            void set_float(const std::string& name, float value) const;
            void set_mat4(const std::string& name, const glm::mat4& value) const;

            void reload();

        private:
            GLuint m_id = 0;
            std::filesystem::path m_vertShaderFSPath;
            std::filesystem::path m_fragShaderFSPath;
        };
    }
}
