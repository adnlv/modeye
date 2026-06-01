#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Core/Gfx.hpp"
#include "Core/Log.hpp"
#include "Core/Timer.hpp"
#include <cassert>

struct State
{
    Modeye::Timer timer;
    Modeye::Gfx::Camera camera;

    bool first_mouse = true;
    float last_x = 720.0f / 2.0f;
    float last_y = 480.0f / 2.0f;
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

    Modeye::Log::info("Loaded {} vertices from '{}'", out_vertices.size(), path);
}

int main(int argc, char** argv)
{
    Modeye::Log::init();

    Modeye::Gfx::Window window(720, 480, "Modeye");
    window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window.setKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (action == GLFW_PRESS)
            {
                if (key == GLFW_KEY_ESCAPE)
                {
                    glfwSetWindowShouldClose(window, true);
                }
                else if (key == GLFW_KEY_R)
                {
                    // TODO: implement
                }
            }
        });
    window.setCursorPosCallback([](GLFWwindow* window, double xposIn, double yposIn)
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

            state.camera.processMouseMovement(xoffset, yoffset);
        });
    window.setScrollCallback([](GLFWwindow* window, double xoffset, double yoffset)
        {
            state.camera.processMouseScroll(static_cast<float>(yoffset));
        });

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    loadOBJ("assets\\monkey.obj", vertices, uvs, normals);

    std::vector<Modeye::Gfx::Vertex> newVertices(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
        newVertices.at(i).position = vertices.at(i);
        newVertices.at(i).normal = normals.at(i);
        newVertices.at(i).uv = uvs.at(i);
    }

    Modeye::Gfx::Mesh monkeyMesh(newVertices);
    Modeye::Gfx::Shader shader("triangle.vert", "triangle.frag");

    while (!window.shouldClose())
    {
        state.timer.startFrame();

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float dt = state.timer.deltaTime();

        if (window.getKey(GLFW_KEY_W))
        {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Forward, dt);
        }
        else if (window.getKey(GLFW_KEY_S))
        {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Backward, dt);
        }
        if (window.getKey(GLFW_KEY_D))
        {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Right, dt);
        }
        else if (window.getKey(GLFW_KEY_A))
        {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Left, dt);
        }
        if (window.getKey(GLFW_KEY_SPACE))
        {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Up, dt);
        }
        else if (window.getKey(GLFW_KEY_C))
        {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Down, dt);
        }

        shader.setMat4("u_projection", state.camera.getProjectionMat(window.aspectRatio()));
        shader.setMat4("u_model", state.camera.getViewMat());
        shader.setMat4("u_view", glm::mat4(1.0f));
        shader.use();

        monkeyMesh.draw(GL_TRIANGLES);

        window.pollEvents();
        window.swapBuffers();

        state.timer.endFrame();
    }

    return 0;
}
