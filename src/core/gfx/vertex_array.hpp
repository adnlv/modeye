#pragma once
#include "vertex_buffer.hpp"
#include <glad/gl.h>

namespace modeye
{
    namespace gfx
    {
        class vertex_array
        {
        public:
            vertex_array();
            ~vertex_array();

            vertex_array(const vertex_array&) = delete;
            vertex_array& operator=(const vertex_array&) = delete;

            vertex_array(vertex_array&& other) noexcept;
            vertex_array& operator=(vertex_array&& other) noexcept;

            static void unbind();

            void bind() const;
            void link_attrib(
                const vertex_buffer& vbo,
                GLuint layout,
                GLuint num_components,
                GLenum type,
                GLsizei stride,
                const void* offset
            ) const;

            GLuint id() const;

        private:
            GLuint m_id;
        };
    }
}