#pragma once
#include <glm/glm.hpp>

namespace Modeye::Gfx
{

struct Vertex
{
    glm::vec3 position{ 0.0f };
    glm::vec3 normal{ 0.0f };
    glm::vec2 uv{ 0.0f };
    glm::vec3 color{ 1.0f };

    bool operator==(const Vertex& other) const
    {
        return position == other.position && normal == other.normal && uv == other.uv && color == other.color;
    }
};

}
