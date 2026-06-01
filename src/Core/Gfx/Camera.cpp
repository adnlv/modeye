#include "../Log.hpp"
#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Modeye::Gfx::Camera::Camera(glm::vec3 position)
    : m_position(position), m_pitch(0.0f), m_yaw(-90.0f), m_fov(45.f)
{
    updateVectors();
}

glm::mat4 Modeye::Gfx::Camera::getViewMat() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Modeye::Gfx::Camera::getProjectionMat(float aspect) const
{
    return glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
}

void Modeye::Gfx::Camera::processKeyboard(Modeye::Gfx::Camera::Direction direction, float deltaTime)
{
    const float velocity = m_speed * deltaTime;

    switch (direction)
    {
    case Modeye::Gfx::Camera::Direction::Forward:
        m_position += m_front * velocity;
        break;
    case Modeye::Gfx::Camera::Direction::Backward:
        m_position -= m_front * velocity;
        break;
    case Modeye::Gfx::Camera::Direction::Right:
        m_position += m_right * velocity;
        break;
    case Modeye::Gfx::Camera::Direction::Left:
        m_position -= m_right * velocity;
        break;
    case Modeye::Gfx::Camera::Direction::Up:
        m_position += m_up * velocity;
        break;
    case Modeye::Gfx::Camera::Direction::Down:
        m_position -= m_up * velocity;
        break;
    default:
        break;
    }
}

void Modeye::Gfx::Camera::processMouseMovement(float xOffset, float yOffset)
{
    m_yaw -= xOffset * m_sensivity;
    m_pitch += yOffset * m_sensivity;

    if (m_pitch > 89.0f)
    {
        m_pitch = 89.0f;
    }
    else if (m_pitch < -89.0f)
    {
        m_pitch = -89.0f;
    }

    updateVectors();
}

void Modeye::Gfx::Camera::processMouseScroll(float yOffset)
{
    m_fov -= yOffset;

    if (m_fov > 45.0f)
    {
        m_fov = 45.0f;
    }
    else if (m_fov < 1.0f)
    {
        m_fov = 1.0f;
    }
    else
    {
        Modeye::Log::info("Camera FOV changed to: {:.1f} deg (Zoom factor: {:.1f}x)", m_fov, 45.0f / m_fov);
    }
}

void Modeye::Gfx::Camera::updateVectors()
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
