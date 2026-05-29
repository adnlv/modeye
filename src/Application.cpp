#define _USE_MATH_DEFINES
#include <cmath>

#include <cassert>
#include <iostream>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include "Timer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct State
{
    Timer* timer = nullptr;

    const float cam_speed = 1.0f;
    const float mouse_speed = 0.05f;

    float cam_fov = 45.0f;
    glm::vec3 cam_pos = glm::vec3(0, 0, 3.0f);
    glm::vec2 cam_angle_deg = glm::vec2(3.14f, 0);
    glm::vec2 mouse_pos = glm::vec2(0, 0);
    glm::vec2 mouse_wheel_offset = glm::vec2(0, 0);

    bool first_mouse = true;
    float last_x = 720.0f / 2.0f;
    float last_y = 480.0f / 2.0f;

    glm::vec3 direction = glm::vec3(0, 0, 0);
    glm::vec3 right = glm::vec3(0, 0, 0);
} state;

int main(int argc, char** argv)
{
    glfwSetErrorCallback(
        [](int error_code, const char* description)
        {
            std::cerr
                << "ERROR::GLFW ("
                << error_code
                << ") >> "
                << description
                << std::endl;
        });

    assert(glfwInit());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(720, 480, "ModEye", nullptr, nullptr);
    assert(window != nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    assert(gladLoadGL(glfwGetProcAddress) != 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        });

    float points[] = {
        0.0f, 0.5f, 0.0f, // Top corner
        0.5f, -0.5f, 0.0f, // Bottom-right corner
        -0.5f, -0.5f, 0.0f // Bottom-left corner
    };

    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, reinterpret_cast<void*>(0));

    GLuint colors_vbo;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, reinterpret_cast<void*>(0));

    Shader shader("triangle.vert", "triangle.frag");
    glfwSetWindowUserPointer(window, &shader);

    glfwSetKeyCallback(window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            const float dt = static_cast<float>(state.timer->deltaTime());

            if (action == GLFW_PRESS)
            {
                if (key == GLFW_KEY_ESCAPE)
                {
                    glfwSetWindowShouldClose(window, true);
                }
                else if (key == GLFW_KEY_R)
                {
                    Shader* shader = reinterpret_cast<Shader*>(glfwGetWindowUserPointer(window));
                    shader->reload();
                }
            }
        });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window,
        [](GLFWwindow* window, double xposIn, double yposIn)
        {
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (state.first_mouse) {
                state.last_x = xpos;
                state.last_y = ypos;
                state.first_mouse = false;
            }

            // Reversed since coordinates go from bottom to top
            float xoffset = state.last_x - xpos;
            float yoffset = state.last_y - ypos;

            state.last_x = xpos;
            state.last_y = ypos;

            const float dt = static_cast<float>(state.timer->deltaTime());
            state.cam_angle_deg.x += xoffset * state.mouse_speed * dt;
            state.cam_angle_deg.y += yoffset * state.mouse_speed * dt;
        });
    glfwSetScrollCallback(window,
        [](GLFWwindow* window, double xoffset, double yoffset)
        {
            state.cam_fov -= static_cast<float>(yoffset) * 2.0f;
            if (state.cam_fov < 1.0f) state.cam_fov = 1.0f;
            if (state.cam_fov > 45.0f) state.cam_fov = 45.0f;
            std::cout << "FOV: " << state.cam_fov << std::endl;
        });

    Timer timer;
    state.timer = &timer;
    while (!glfwWindowShouldClose(window))
    {
        timer.startFrame();

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        state.direction = glm::vec3(
            cos(state.cam_angle_deg.y) * sin(state.cam_angle_deg.x),
            sin(state.cam_angle_deg.y),
            cos(state.cam_angle_deg.y) * cos(state.cam_angle_deg.x)
        );

        state.right = glm::vec3(
            sin(state.cam_angle_deg.x - M_PI / 2.f),
            0,
            cos(state.cam_angle_deg.x - M_PI / 2.f)
        );

        glm::vec3 up = glm::cross(state.right, state.direction);

        const float dt = static_cast<float>(state.timer->deltaTime());
        if (glfwGetKey(window, GLFW_KEY_W))
        {
            state.cam_pos += state.direction * state.cam_speed * dt;
            std::cout << "FORWARD\n";
        } else if (glfwGetKey(window, GLFW_KEY_S))
        {
            state.cam_pos -= state.direction * state.cam_speed * dt;
            std::cout << "BACKWARD\n";
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            state.cam_pos -= state.right * state.cam_speed * dt;
            std::cout << "LEFT\n";
        } else if (glfwGetKey(window, GLFW_KEY_D))
        {
            state.cam_pos += state.right * state.cam_speed * dt;
            std::cout << "RIGHT\n";
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            state.cam_pos += up * state.cam_speed * dt;
            std::cout << "UP\n";
        } else if (glfwGetKey(window, GLFW_KEY_C))
        {
            state.cam_pos -= up * state.cam_speed * dt;
            std::cout << "DOWN\n";
        }
        
        glm::mat4 projection = glm::perspective(glm::radians(state.cam_fov), 3.f / 4.f, 0.1f, 100.f);
        glm::mat4 view = glm::lookAt(state.cam_pos, state.cam_pos + state.direction, up);
        glm::mat4 model = glm::mat4(1.0f);

        shader.use();

        GLuint projectionLocation = glGetUniformLocation(shader.id(), "u_projection");
        GLuint modelLocation = glGetUniformLocation(shader.id(), "u_model");
        GLuint viewLocation = glGetUniformLocation(shader.id(), "u_view");
        glUniformMatrix4fv(projectionLocation, 1, false, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLocation, 1, false, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, false, glm::value_ptr(view));

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window);

        timer.endFrame();
    }

    glfwTerminate();
    return 0;
}
