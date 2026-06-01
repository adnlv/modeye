#pragma once
#include <glm/glm.hpp>

namespace Modeye::Gfx
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };
}
