#include "IndexBuffer.hpp"
#include "../Log.hpp"

Modeye::Gfx::IndexBuffer::IndexBuffer(size_t count, const uint32_t* data, GLenum usage)
    : m_id(0), m_count(static_cast<GLsizei>(count))
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(count * sizeof(uint32_t)), data, usage);

    Modeye::Log::trace("Generated index buffer (ID: {}, count: {})", m_id, m_count);
}

Modeye::Gfx::IndexBuffer::~IndexBuffer()
{
    if (m_id != 0) {
        Modeye::Log::trace("Deleted index buffer (ID: {})", m_id);
        glDeleteBuffers(1, &m_id);
    }
}

Modeye::Gfx::IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
    : m_id(other.m_id), m_count(other.m_count)
{
    other.m_id = 0;
    other.m_count = 0;
}

Modeye::Gfx::IndexBuffer& Modeye::Gfx::IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
    if (this != &other) {
        if (m_id != 0) {
            glDeleteBuffers(1, &m_id);
        }

        m_id = other.m_id;
        m_count = other.m_count;
        other.m_id = 0;
        other.m_count = 0;
    }

    return *this;
}

void Modeye::Gfx::IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Modeye::Gfx::IndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

GLuint Modeye::Gfx::IndexBuffer::id() const
{
    return m_id;
}

GLsizei Modeye::Gfx::IndexBuffer::count() const
{
    return m_count;
}
