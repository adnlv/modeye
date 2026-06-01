#include "Camera.hpp"
#include "Log.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Timer.hpp"
#include <cassert>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct State
{
    Timer timer;
    Camera camera;

    bool first_mouse = true;
    float last_x = 720.0f / 2.0f;
    float last_y = 480.0f / 2.0f;

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

static void printSystemInfo()
{
    Log::info("OpenGL context created:");
    Log::info("  GPU vendor:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    Log::info("  GPU renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    Log::info("  GL version:   {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Log::info("  GLSL version: {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    GLint maxTextureSize = 0;
    GLint maxMsaaSamples = 0;
    GLint maxVertexAttribs = 0;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_SAMPLES, &maxMsaaSamples);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    Log::info("GPU hardware capability limits:");
    Log::info("  Max 2D texture size:   {}x{}", maxTextureSize, maxTextureSize);
    Log::info("  Max MSAA samples:      {}x", maxMsaaSamples);
    Log::info("  Max vertex attributes: {}", maxVertexAttribs);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (primaryMonitor)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        Log::info("Primary monitor detected: {}x{} @ {}Hz", mode->width, mode->height, mode->refreshRate);
    }
}

static GLFWwindow* loadGfx()
{
    glfwSetErrorCallback(
        [](int error_code, const char* description)
        {
            Log::error("GLFW ({}) >> {}", error_code, description);
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
    //glfwSwapInterval(1);

    assert(gladLoadGL(glfwGetProcAddress) != 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    printSystemInfo();

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
            state.window_width = width;
            state.window_height = height;
            state.screen_aspect_ratio = static_cast<float>(state.window_width) / static_cast<float>(state.window_height);

            Log::info("Framebuffer resized to: {}x{} | Aspect Ratio: {:.3f}", width, height, state.screen_aspect_ratio);
        });

    glfwSetKeyCallback(window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
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

            state.camera.processMouseMovement(xoffset, yoffset);
        });
    
    glfwSetScrollCallback(window,
        [](GLFWwindow* window, double xoffset, double yoffset)
        {
            state.camera.processMouseScroll(static_cast<float>(yoffset));
        });

    return window;
}

int main(int argc, char** argv)
{
    Log::init();

    GLFWwindow* window = loadGfx();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    loadOBJ("assets\\monkey.obj", vertices, uvs, normals);

    std::vector<Vertex> newVertices(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
        newVertices.at(i).position = vertices.at(i);
        newVertices.at(i).normal = normals.at(i);
        newVertices.at(i).uv = uvs.at(i);
    }

    Mesh monkeyMesh(newVertices);

    Shader shader("triangle.vert", "triangle.frag");
    glfwSetWindowUserPointer(window, &shader);

    while (!glfwWindowShouldClose(window))
    {
        state.timer.startFrame();

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float dt = static_cast<float>(state.timer.deltaTime());

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            state.camera.processKeyboard(Camera::Direction::FORWARD, dt);
        }
        else if (glfwGetKey(window, GLFW_KEY_S))
        {
            state.camera.processKeyboard(Camera::Direction::BACKWARD, dt);
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            state.camera.processKeyboard(Camera::Direction::RIGHT, dt);
        }
        else if (glfwGetKey(window, GLFW_KEY_A))
        {
            state.camera.processKeyboard(Camera::Direction::LEFT, dt);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            state.camera.processKeyboard(Camera::Direction::UP, dt);
        }
        else if (glfwGetKey(window, GLFW_KEY_C))
        {
            state.camera.processKeyboard(Camera::Direction::DOWN, dt);
        }

        shader.use();

        const glm::mat4 projection = state.camera.getProjectionMatrix(state.screen_aspect_ratio);
        const glm::mat4 view = state.camera.getViewMatrix();
        const glm::mat4 model = glm::mat4(1.0f);

        shader.setMat4("u_projection", projection);
        shader.setMat4("u_model", model);
        shader.setMat4("u_view", view);

        monkeyMesh.draw(GL_TRIANGLES);

        glfwPollEvents();
        glfwSwapBuffers(window);

        state.timer.endFrame();
    }

    glfwTerminate();
    return 0;
}
