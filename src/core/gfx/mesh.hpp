#pragma once
#include "vertex.hpp"
#include "vertex_array.hpp"

namespace modeye
{
    namespace gfx
    {
        class mesh
        {
        public:
            mesh(const std::vector<modeye::gfx::vertex>& vertices);
            ~mesh() = default;

            mesh(const mesh&) = delete;
            mesh& operator=(const mesh&) = delete;

            mesh(mesh&&) noexcept = default;
            mesh& operator=(mesh&&) noexcept = default;

            void draw(GLenum mode) const;

            const modeye::gfx::vertex_array& vertex_array() const;
            const modeye::gfx::vertex_buffer& vertex_buffer() const;

        private:
            modeye::gfx::vertex_array m_vertexArray;
            modeye::gfx::vertex_buffer m_vertexBuffer;
            GLsizei m_vertexCount;
        };
    }
}
