#pragma once
#include <glm/glm.hpp>

namespace modeye
{
    namespace gfx
    {
        struct vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
        };
    }
}
