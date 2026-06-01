#include "../log.hpp"
#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

modeye::gfx::camera::camera(glm::vec3 position)
    : m_position(position), m_pitch(0.0f), m_yaw(-90.0f), m_fov(45.f)
{
    update_vectors();
}

glm::mat4 modeye::gfx::camera::get_view_mat() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 modeye::gfx::camera::get_projection_mat(float aspect) const
{
    return glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
}

void modeye::gfx::camera::process_keyboard(modeye::gfx::camera::direction direction, float delta_time)
{
    const float velocity = m_speed * delta_time;

    switch (direction)
    {
    case modeye::gfx::camera::FORWARD:
        m_position += m_front * velocity;
        break;
    case modeye::gfx::camera::BACKWARD:
        m_position -= m_front * velocity;
        break;
    case modeye::gfx::camera::RIGHT:
        m_position += m_right * velocity;
        break;
    case modeye::gfx::camera::LEFT:
        m_position -= m_right * velocity;
        break;
    case modeye::gfx::camera::UP:
        m_position += m_up * velocity;
        break;
    case modeye::gfx::camera::DOWN:
        m_position -= m_up * velocity;
        break;
    default:
        break;
    }
}

void modeye::gfx::camera::process_mouse_movement(float offset_x, float offset_y)
{
    m_yaw -= offset_x * m_sensivity;
    m_pitch += offset_y * m_sensivity;

    if (m_pitch > 89.0f)
    {
        m_pitch = 89.0f;
    }
    else if (m_pitch < -89.0f)
    {
        m_pitch = -89.0f;
    }

    update_vectors();
}

void modeye::gfx::camera::process_mouse_scroll(float offset_y)
{
    m_fov -= offset_y;

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
        modeye::log::info("Camera FOV changed to: {:.1f} deg (Zoom factor: {:.1f}x)", m_fov, 45.0f / m_fov);
    }
}

void modeye::gfx::camera::update_vectors()
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
