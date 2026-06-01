#include "../log.hpp"
#include "vertex_array.hpp"

modeye::gfx::vertex_array::vertex_array()
    : m_id(0)
{
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);

    modeye::log::trace("Generated vertex array (ID: {})", m_id);
}

modeye::gfx::vertex_array::~vertex_array()
{
    if (m_id != 0)
    {
        modeye::log::trace("Deleted vertex array (ID: {})", m_id);

        glDeleteVertexArrays(1, &m_id);
    }
}

modeye::gfx::vertex_array::vertex_array(vertex_array&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

modeye::gfx::vertex_array& modeye::gfx::vertex_array::operator=(vertex_array&& other) noexcept
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

void modeye::gfx::vertex_array::unbind()
{
    glBindVertexArray(0);
}

void modeye::gfx::vertex_array::bind() const
{
    glBindVertexArray(m_id);
}

void modeye::gfx::vertex_array::link_attrib(const vertex_buffer& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizei stride, const void* offset) const
{
    bind();
    vbo.bind();

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);

    vertex_buffer::unbind();
    vertex_array::unbind();
}

GLuint modeye::gfx::vertex_array::id() const
{
    return m_id;
}
