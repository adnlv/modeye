#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position)
    : m_position(position), m_pitch(0.0f), m_yaw(-90.0f), m_fov(45.f)
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const
{
    return glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
}

void Camera::processKeyboard(Direction direction, float deltaTime)
{
    const float velocity = m_speed * deltaTime;

    switch (direction)
    {
    case Camera::FORWARD:
        m_position += m_front * velocity;
        break;
    case Camera::BACKWARD:
        m_position -= m_front * velocity;
        break;
    case Camera::RIGHT:
        m_position += m_right * velocity;
        break;
    case Camera::LEFT:
        m_position -= m_right * velocity;
        break;
    case Camera::UP:
        m_position += m_up * velocity;
        break;
    case Camera::DOWN:
        m_position -= m_up * velocity;
        break;
    default:
        break;
    }
}

void Camera::processMouseMovement(float offset_x, float offset_y)
{
    m_yaw -= offset_x * m_sensivity;
    m_pitch += offset_y * m_sensivity;
   
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    else if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateVectors();
}

void Camera::processMouseScroll(float offset_y)
{
    m_fov -= offset_y;

    if (m_fov > 45.0f) m_fov = 45.0f;
    else if (m_fov < 1.0f) m_fov = 1.0f;
}

void Camera::updateVectors()
{
    const float pitch_rad = glm::radians(m_pitch);
    const float yaw_rad = glm::radians(m_yaw);

    glm::vec3 front(
        cos(pitch_rad) * cos(yaw_rad),
        sin(pitch_rad),
        cos(pitch_rad) * sin(yaw_rad)
    );
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
