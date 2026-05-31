#pragma once
#include <glad/gl.h>

class VertexBuffer
{
public:
    VertexBuffer(size_t size, const void* data, GLenum usage);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void bind() const;

    static void unbind();

    GLuint id() const;

private:
    GLuint m_id;
};
