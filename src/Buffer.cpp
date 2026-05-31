#include "Buffer.hpp"
#include "Log.hpp"

VertexBuffer::VertexBuffer(size_t size, const void* data, GLenum usage)
    : m_id(0)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);

    Log::trace("Generated vertex buffer (ID: {})", m_id);
}

VertexBuffer::~VertexBuffer()
{
    if (m_id != 0)
    {
        Log::trace("Deleted vertex buffer (ID: {})", m_id);

        glDeleteBuffers(1, &m_id);
    }
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_id != 0)
        {
            glDeleteBuffers(1, &m_id);
        }

        m_id = other.m_id;
        other.m_id = 0;
    }

    return *this;
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint VertexBuffer::id() const
{
    return m_id;
}

VertexArray::VertexArray()
    : m_id(0)
{
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
 
    Log::trace("Generated vertex array (ID: {})", m_id);
}

VertexArray::~VertexArray()
{
    if (m_id != 0)
    {
        Log::trace("Deleted vertex array (ID: {})", m_id);

        glDeleteVertexArrays(1, &m_id);
    }
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        if (m_id != 0)
        {
            glDeleteVertexArrays(1, &m_id);
        }

        m_id = other.m_id;
        other.m_id = 0;
    }

    return *this;
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::linkAttrib(const VertexBuffer& vertexBuffer, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, const void* offset) const
{
    bind();
    vertexBuffer.bind();

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);

    VertexBuffer::unbind();
    VertexArray::unbind();
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

GLuint VertexArray::id() const
{
    return m_id;
}
