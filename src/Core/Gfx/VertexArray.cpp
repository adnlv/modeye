#include "../Log.hpp"
#include "VertexArray.hpp"

Modeye::Gfx::VertexArray::VertexArray()
    : m_id(0)
{
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);

    Modeye::Log::trace("Generated vertex array (ID: {})", m_id);
}

Modeye::Gfx::VertexArray::~VertexArray()
{
    if (m_id != 0) {
        Modeye::Log::trace("Deleted vertex array (ID: {})", m_id);

        glDeleteVertexArrays(1, &m_id);
    }
}

Modeye::Gfx::VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

Modeye::Gfx::VertexArray& Modeye::Gfx::VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other) {
        if (m_id != 0) {
            glDeleteVertexArrays(1, &m_id);
        }

        m_id = other.m_id;
        other.m_id = 0;
    }

    return *this;
}

void Modeye::Gfx::VertexArray::unbind()
{
    glBindVertexArray(0);
}

void Modeye::Gfx::VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void Modeye::Gfx::VertexArray::linkAttrib(const VertexBuffer& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizei stride, const void* offset) const
{
    bind();
    vbo.bind();

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);

    VertexBuffer::unbind();
    VertexArray::unbind();
}

GLuint Modeye::Gfx::VertexArray::id() const
{
    return m_id;
}
