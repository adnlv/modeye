#pragma once
#include <glm/glm.hpp>

namespace Modeye::Gfx
{
    class Camera
    {
    public:
        enum class Direction
        {
            Forward,
            Backward,
            Right,
            Left,
            Up,
            Down
        };

        Camera(glm::vec3 position = glm::vec3(0, 0, 5.0f));
        ~Camera() = default;

        glm::mat4 getViewMat() const;
        glm::mat4 getProjectionMat(float aspect) const;

        void processKeyboard(Direction direction, float deltaTime);
        void processMouseMovement(float xOffset, float yOffset);
        void processMouseScroll(float yOffset);

    private:
        glm::vec3 m_position;
        glm::vec3 m_right;
        glm::vec3 m_front;
        glm::vec3 m_up;

        float m_pitch;
        float m_yaw;
        float m_fov;
        float m_speed = 5.0f;
        float m_sensivity = 0.05f;

        void updateVectors();
    };
}

