#pragma once
#include <glad/gl.h>

namespace modeye
{
    namespace gfx
    {
        class vertex_buffer
        {
        public:
            vertex_buffer(size_t size, const void* data, GLenum usage);
            ~vertex_buffer();

            vertex_buffer(const vertex_buffer&) = delete;
            vertex_buffer& operator=(const vertex_buffer&) = delete;

            vertex_buffer(vertex_buffer&& other) noexcept;
            vertex_buffer& operator=(vertex_buffer&& other) noexcept;

            static void unbind();
            
            void bind() const;

            GLuint id() const;

        private:
            GLuint m_id;
        };
    }
}
