#pragma once
#include <glad/gl.h>

namespace Modeye::Gfx
{

class VertexBuffer
{
public:
    VertexBuffer(size_t size, const void* data, GLenum usage);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    static void unbind();

    void bind() const;

    GLuint id() const;

private:
    GLuint m_id;
};

}
