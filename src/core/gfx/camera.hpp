#pragma once
#include <glm/glm.hpp>

namespace modeye
{
    namespace gfx
    {
        class camera
        {
        public:
            enum direction
            {
                FORWARD,
                BACKWARD,
                RIGHT,
                LEFT,
                UP,
                DOWN
            };

            camera(glm::vec3 position = glm::vec3(0, 0, 5.0f));
            ~camera() = default;

            glm::mat4 get_view_mat() const;
            glm::mat4 get_projection_mat(float aspect) const;

            void process_keyboard(direction direction, float delta_time);
            void process_mouse_movement(float offset_x, float offset_y);
            void process_mouse_scroll(float offset_y);

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

            void update_vectors();
        };
    }
}

