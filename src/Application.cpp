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

    const float cam_speed = 5.0f;
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

    int window_width = 720;
    int window_height = 480;
    float screen_aspect_ratio = 1;
} state;

void loadOBJ(
    const std::string& path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals)
{
    std::vector<GLuint> vertex_idxs, uv_idxs, normal_idxs;
    std::vector<glm::vec3> temp_vertices, temp_normals;
    std::vector<glm::vec2> temp_uvs;

    FILE* file = fopen(path.c_str(), "r");
    assert(file != nullptr);

    while (true)
    {
        char header[128];
        int res = fscanf(file, "%s", header);
        if (res == EOF)
        {
            break;
        }

        if (strcmp(header, "v") == 0)
        {
            glm::vec3 v{};
            (void)fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
            temp_vertices.push_back(v);
        }
        else if (strcmp(header, "vt") == 0)
        {
            glm::vec2 uv{};
            (void)fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(header, "vn") == 0)
        {
            glm::vec3 n{};
            (void)fscanf(file, "%f %f %f\n", &n.x, &n.y, &n.z);
            temp_normals.push_back(n);
        }
        else if (strcmp(header, "f") == 0)
        {
            std::string v1, v2, v3;
            unsigned int v_idx[3], uv_idx[3], n_idx[3];

            int matches = fscanf(
                file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &v_idx[0], &uv_idx[0], &n_idx[0],
                &v_idx[1], &uv_idx[1], &n_idx[1],
                &v_idx[2], &uv_idx[2], &n_idx[2]);
            assert(matches == 9);

            vertex_idxs.push_back(v_idx[0]);
            vertex_idxs.push_back(v_idx[1]);
            vertex_idxs.push_back(v_idx[2]);

            uv_idxs.push_back(uv_idx[0]);
            uv_idxs.push_back(uv_idx[1]);
            uv_idxs.push_back(uv_idx[2]);

            normal_idxs.push_back(n_idx[0]);
            normal_idxs.push_back(n_idx[1]);
            normal_idxs.push_back(n_idx[2]);
        }
    }

    for (size_t i = 0; i < vertex_idxs.size(); ++i)
    {
        GLuint v_idx = vertex_idxs[i];
        glm::vec3 v = temp_vertices[v_idx - 1];
        out_vertices.push_back(v);
    }

    for (size_t i = 0; i < uv_idxs.size(); ++i)
    {
        GLuint uv_idx = uv_idxs[i];
        glm::vec2 uv = temp_uvs[uv_idx - 1];
        out_uvs.push_back(uv);
    }
    
    for (size_t i = 0; i < normal_idxs.size(); ++i)
    {
        GLuint n_idx = normal_idxs[i];
        glm::vec3 n = temp_normals[n_idx - 1];
        out_normals.push_back(n);
    }
}

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

    GLFWwindow* window = glfwCreateWindow(state.window_width, state.window_height, "ModEye", nullptr, nullptr);
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
            state.window_width = width;
            state.window_height = height;
            state.screen_aspect_ratio = static_cast<float>(state.window_width) / static_cast<float>(state.window_height);
        });

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    loadOBJ("assets\\dragon.obj", vertices, uvs, normals);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, reinterpret_cast<void*>(0));

    GLuint colors_vbo;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

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
        }
        else if (glfwGetKey(window, GLFW_KEY_S))
        {
            state.cam_pos -= state.direction * state.cam_speed * dt;
            std::cout << "BACKWARD\n";
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            state.cam_pos -= state.right * state.cam_speed * dt;
            std::cout << "LEFT\n";
        }
        else if (glfwGetKey(window, GLFW_KEY_D))
        {
            state.cam_pos += state.right * state.cam_speed * dt;
            std::cout << "RIGHT\n";
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            state.cam_pos += up * state.cam_speed * dt;
            std::cout << "UP\n";
        }
        else if (glfwGetKey(window, GLFW_KEY_C))
        {
            state.cam_pos -= up * state.cam_speed * dt;
            std::cout << "DOWN\n";
        }

        glm::mat4 projection = glm::perspective(
            glm::radians(state.cam_fov), 
            state.screen_aspect_ratio, 
            0.1f, 100.f);
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
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwPollEvents();
        glfwSwapBuffers(window);

        timer.endFrame();
    }

    glfwTerminate();
    return 0;
}
