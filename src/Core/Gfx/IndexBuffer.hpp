#pragma once
#include "../GL.hpp"
#include <cstdint>

namespace Modeye::Gfx
{

class IndexBuffer
{
public:
    IndexBuffer(size_t count, const uint32_t* data, GLenum usage = GL_STATIC_DRAW);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&& other) noexcept;
    IndexBuffer& operator=(IndexBuffer&& other) noexcept;

    static void unbind();

    void bind() const;

    GLuint id() const;
    GLsizei count() const;

private:
    GLuint m_id;
    GLsizei m_count;
};

}
