#pragma once
#include "VertexBuffer.hpp"
#include <glad/gl.h>

namespace Modeye::Gfx
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        static void unbind();

        void bind() const;
        void linkAttrib(
            const VertexBuffer& vbo,
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