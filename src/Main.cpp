#include "Core/Gfx.hpp"
#include "Core/Log.hpp"
#include "Core/Timer.hpp"
#include <fstream>
#include <sstream>

struct State
{
    Modeye::Timer timer;
    Modeye::Gfx::Camera camera;

    bool first_mouse = true;
    float last_x = 720.0f / 2.0f;
    float last_y = 480.0f / 2.0f;
} state;

static auto loadObj(const std::filesystem::path& path) -> std::vector<Modeye::Gfx::Vertex>
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open OBJ file: " + path.string());
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_uvs;

    struct FaceIndex
    {
        GLuint v = 0;
        GLuint vt = 0;
        GLuint vn = 0;
    };

    std::vector<FaceIndex> face_indices;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 v;
            if (ss >> v.x >> v.y >> v.z) {
                temp_vertices.push_back(v);
            }
        } else if (prefix == "vt") {
            glm::vec2 uv;
            if (ss >> uv.x >> uv.y) {
                temp_uvs.push_back(uv);
            }
        } else if (prefix == "vn") {
            glm::vec3 n;
            if (ss >> n.x >> n.y >> n.z) {
                temp_normals.push_back(n);
            }
        } else if (prefix == "f") {
            FaceIndex f[3];
            char slash;
            bool parse_success = true;

            // Parsers the standard format: v/vt/vn v/vt/vn v/vt/vn
            for (int i = 0; i < 3; ++i) {
                if (!(ss >> f[i].v >> slash >> f[i].vt >> slash >> f[i].vn)) {
                    parse_success = false;
                    break;
                }
            }
            if (parse_success) {
                face_indices.push_back(f[0]);
                face_indices.push_back(f[1]);
                face_indices.push_back(f[2]);
            } else {
                Modeye::Log::warn("Skipped malformed face line: '{}'", line);
            }
        }
    }

    std::vector<Modeye::Gfx::Vertex> vertices;
    vertices.reserve(face_indices.size());

    for (const auto& idx : face_indices) {
        Modeye::Gfx::Vertex vertex{};
        // OBJ indices are 1-based; bounds checks are applied to prevent segfaults

        if (idx.v > 0 && static_cast<size_t>(idx.v) <= temp_vertices.size()) {
            vertex.position = temp_vertices[idx.v - 1];
        }

        if (idx.vt > 0 && static_cast<size_t>(idx.vt) <= temp_uvs.size()) {
            vertex.uv = temp_uvs[idx.vt - 1];
        }

        if (idx.vn > 0 && static_cast<size_t>(idx.vn) <= temp_normals.size()) {
            vertex.normal = temp_normals[idx.vn - 1];
        }

        vertices.push_back(vertex);
    }

    Modeye::Log::info("Loaded {} vertices from '{}'", vertices.size(), path.string());

    return vertices;
}

int main(int argc, char** argv)
{
    Modeye::Log::init();

    Modeye::Gfx::Window window(720, 480, "Modeye");
    window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window.setKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (action == GLFW_PRESS) {
                if (key == GLFW_KEY_ESCAPE) {
                    glfwSetWindowShouldClose(window, true);
                } else if (key == GLFW_KEY_R) {
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

    auto vertices = loadObj("assets\\monkey.obj");
    auto monkeyMesh = Modeye::Gfx::Mesh(vertices);
    auto shader = Modeye::Gfx::Shader("triangle.vert", "triangle.frag");

    while (!window.shouldClose()) {
        state.timer.startFrame();

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float dt = state.timer.deltaTime();

        if (window.getKey(GLFW_KEY_W)) {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Forward, dt);
        } else if (window.getKey(GLFW_KEY_S)) {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Backward, dt);
        }
        if (window.getKey(GLFW_KEY_D)) {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Right, dt);
        } else if (window.getKey(GLFW_KEY_A)) {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Left, dt);
        }
        if (window.getKey(GLFW_KEY_SPACE)) {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Up, dt);
        } else if (window.getKey(GLFW_KEY_C)) {
            state.camera.processKeyboard(Modeye::Gfx::Camera::Direction::Down, dt);
        }

        shader.use();

        auto model = glm::mat4(1.0f);

        shader.setMat4("u_projection", state.camera.getProjectionMat(window.aspectRatio()));
        shader.setMat4("u_view", state.camera.getViewMat());
        shader.setMat4("u_model", model);

        monkeyMesh.draw(GL_TRIANGLES);

        window.pollEvents();
        window.swapBuffers();

        state.timer.endFrame();
    }

    return 0;
}
