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

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void bind() const;
    void linkAttrib(
        const VertexBuffer& vertexBuffer,
        GLuint layout,
        GLuint numComponents,
        GLenum type, 
        GLsizei stride, 
        const void* offset
    ) const;
    
    static void unbind();

    GLuint id() const;

private:
    GLuint m_id;
};
