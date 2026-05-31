#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
    enum Direction
    {
        FORWARD,
        BACKWARD,
        RIGHT,
        LEFT,
        UP,
        DOWN
    };

    Camera(glm::vec3 position = glm::vec3(0, 0, 5.0f));
    ~Camera() = default;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;

    void processKeyboard(Direction direction, float deltaTime);
    void processMouseMovement(float offset_x, float offset_y);
    void processMouseScroll(float offset_y);

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
