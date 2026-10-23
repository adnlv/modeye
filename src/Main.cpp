#include "Core/Gfx.hpp"
#include "Core/Log.hpp"
#include "Core/Timer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <vector>

struct AppState
{
    Modeye::Timer timer;
    Modeye::Gfx::Camera camera;

    // Viewport drag camera rotation state
    bool is_rotating_camera = false;
    bool first_mouse_drag = false;
    int drag_button = -1;
    double drag_start_x = 0.0;
    double drag_start_y = 0.0;
    float last_x = 0.0f;
    float last_y = 0.0f;

    bool reload_shader_requested = false;

    // Render parameters
    int current_model_idx = 0; // 0 = Monkey, 1 = Dragon, 2 = Retro TV
    int render_mode = 0;        // 0 = Blinn-Phong, 1 = Normal Visualizer, 2 = Unlit
    bool wireframe = false;
    bool auto_rotate = false;
    float rotation_angle = 0.0f;

    // Lighting parameters
    glm::vec3 light_pos = glm::vec3(2.5f, 4.0f, 3.0f);
    glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 object_color = glm::vec3(0.85f, 0.55f, 0.25f);
    float ambient_strength = 0.20f;
    float specular_strength = 0.65f;
    float shininess = 32.0f;

    bool use_texture = false;
    bool use_material_colors = false;
} state;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    Modeye::Log::init();

    Modeye::Gfx::Window window(1280, 720, "Modeye 3D Graphics Engine");
    window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Set GLFW callbacks before ImGui installs its chaining callbacks
    window.setKeyCallback([]([[maybe_unused]] GLFWwindow* win, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (action == GLFW_PRESS) {
                if (key == GLFW_KEY_ESCAPE) {
                    glfwSetWindowShouldClose(win, true);
                } else if (!io.WantTextInput) {
                    if (key == GLFW_KEY_R) {
                        state.reload_shader_requested = true;
                    } else if (key == GLFW_KEY_Z) {
                        state.wireframe = !state.wireframe;
                    }
                }
            }
        });

    window.setMouseButtonCallback([](GLFWwindow* win, int button, int action, [[maybe_unused]] int mods)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (action == GLFW_PRESS) {
                if (!state.is_rotating_camera && (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)) {
                    if (!io.WantCaptureMouse) {
                        state.is_rotating_camera = true;
                        state.first_mouse_drag = true;
                        state.drag_button = button;

                        glfwGetCursorPos(win, &state.drag_start_x, &state.drag_start_y);
                        state.last_x = static_cast<float>(state.drag_start_x);
                        state.last_y = static_cast<float>(state.drag_start_y);

                        // Capture cursor for infinite viewport rotation
                        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    }
                }
            } else if (action == GLFW_RELEASE) {
                if (state.is_rotating_camera && button == state.drag_button) {
                    state.is_rotating_camera = false;
                    state.drag_button = -1;

                    // Restore cursor visibility and return to initial click position
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    glfwSetCursorPos(win, state.drag_start_x, state.drag_start_y);
                }
            }
        });

    window.setCursorPosCallback([]([[maybe_unused]] GLFWwindow* win, double xposIn, double yposIn)
        {
            if (!state.is_rotating_camera) {
                return;
            }

            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (state.first_mouse_drag) {
                state.last_x = xpos;
                state.last_y = ypos;
                state.first_mouse_drag = false;
                return;
            }

            float xoffset = state.last_x - xpos;
            float yoffset = state.last_y - ypos;

            state.last_x = xpos;
            state.last_y = ypos;

            state.camera.processMouseMovement(xoffset, yoffset);
        });

    window.setScrollCallback([]([[maybe_unused]] GLFWwindow* win, [[maybe_unused]] double xoffset, double yoffset)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse) {
                state.camera.processMouseScroll(static_cast<float>(yoffset));
            }
        });

    window.setWindowFocusCallback([](GLFWwindow* win, int focused)
        {
            if (!focused && state.is_rotating_camera) {
                state.is_rotating_camera = false;
                state.drag_button = -1;
                glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        });

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    // Style adjustments for modern sleek look
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;

    ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Load Shaders
    auto shader = Modeye::Gfx::Shader("mesh.vert", "mesh.frag");

    // Load Models using robust Model class
    Modeye::Log::info("Loading 3D assets...");
    Modeye::Gfx::Model monkeyModel("assets/monkey.obj");
    Modeye::Gfx::Model dragonModel("assets/dragon.obj");
    Modeye::Gfx::Model tvModel("assets/TV.obj");

    // Create default procedural texture
    auto checkerTexture = Modeye::Gfx::Texture::createCheckerboard(256, 16);

    std::vector<float> frameTimes(100, 0.0f);
    int frameTimeIdx = 0;

    while (!window.shouldClose()) {
        state.timer.startFrame();

        window.pollEvents();

        const float dt = state.timer.deltaTime();

        // Keyboard navigation (active unless typing in an ImGui text input field)
        if (!io.WantTextInput) {
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
        }

        // Live shader hot-reload
        if (state.reload_shader_requested) {
            state.reload_shader_requested = false;
            if (!shader.reload()) {
                Modeye::Log::warn("Shader reload failed; continuing with previous shader program");
            }
        }

        if (state.auto_rotate) {
            state.rotation_angle += 45.0f * dt;
            if (state.rotation_angle >= 360.0f) {
                state.rotation_angle -= 360.0f;
            }
        }

        // Start Dear ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui Control Panel Window
        {
            ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(340.0f, 520.0f), ImGuiCond_FirstUseEver);
            ImGui::Begin("Modeye Control Panel", nullptr);

            if (ImGui::CollapsingHeader("Metrics & Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
                float fps = dt > 0.0f ? 1.0f / dt : 0.0f;
                frameTimes[frameTimeIdx] = dt * 1000.0f;
                frameTimeIdx = (frameTimeIdx + 1) % frameTimes.size();

                ImGui::Text("FPS: %.1f", fps);
                ImGui::Text("Frame Time: %.2f ms", dt * 1000.0f);
                ImGui::PlotLines("##frametime", frameTimes.data(), static_cast<int>(frameTimes.size()), frameTimeIdx, "Frame Time (ms)", 0.0f, 35.0f, ImVec2(0, 50));
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Navigation: Hold LMB/RMB to rotate | WASD fly");
            }

            if (ImGui::CollapsingHeader("Model & Geometry", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* models[] = { "Suzanne (Blender Monkey)", "Stanford Dragon", "Retro TV" };
                int previous_model_idx = state.current_model_idx;
                if (ImGui::Combo("Select Model", &state.current_model_idx, models, IM_ARRAYSIZE(models))) {
                    if (state.current_model_idx != previous_model_idx) {
                        if (state.current_model_idx == 2) {
                            state.use_material_colors = true;
                            state.object_color = glm::vec3(1.0f);
                        } else {
                            state.use_material_colors = false;
                            state.object_color = glm::vec3(0.85f, 0.55f, 0.25f);
                        }
                    }
                }

                const Modeye::Gfx::Model& activeModel = (state.current_model_idx == 0) ? monkeyModel :
                                                        (state.current_model_idx == 1) ? dragonModel : tvModel;
                ImGui::Text("Vertices: %zu | Triangles: %zu", activeModel.vertexCount(), activeModel.indexCount() / 3);

                ImGui::Checkbox("Wireframe Mode (Z)", &state.wireframe);
                ImGui::Checkbox("Auto-Rotate Model", &state.auto_rotate);
                ImGui::SliderFloat("Rotation Angle", &state.rotation_angle, 0.0f, 360.0f);
            }

            if (ImGui::CollapsingHeader("Shading & Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* modes[] = { "Blinn-Phong Lighting", "Surface Normals", "Unlit Flat Color" };
                ImGui::Combo("Render Mode", &state.render_mode, modes, IM_ARRAYSIZE(modes));

                ImGui::Checkbox("Use Checkerboard Texture", &state.use_texture);
                ImGui::Checkbox("Use Material Colors (.mtl)", &state.use_material_colors);
                ImGui::ColorEdit3("Object Color / Tint", &state.object_color.x);
                ImGui::SliderFloat("Ambient", &state.ambient_strength, 0.0f, 1.0f);
                ImGui::SliderFloat("Specular", &state.specular_strength, 0.0f, 2.0f);
                ImGui::SliderFloat("Shininess", &state.shininess, 2.0f, 128.0f);
            }

            if (ImGui::CollapsingHeader("Lighting Setup", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::DragFloat3("Light Position", &state.light_pos.x, 0.1f);
                ImGui::ColorEdit3("Light Color", &state.light_color.x);
            }

            if (ImGui::CollapsingHeader("Camera Info")) {
                glm::vec3 camPos = state.camera.position();
                ImGui::Text("Position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
            }

            ImGui::Separator();
            if (ImGui::Button("Reload Shaders (R)")) {
                state.reload_shader_requested = true;
            }

            ImGui::End();
        }

        // Render scene
        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, state.wireframe ? GL_LINE : GL_FILL);

        shader.use();

        auto model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(state.rotation_angle), glm::vec3(0.0f, 1.0f, 0.0f));

        if (state.current_model_idx == 1) {
            // Stanford dragon has different dimensions
            model = glm::scale(model, glm::vec3(0.8f));
        } else if (state.current_model_idx == 2) {
            // Retro TV: orient to face front camera and center at origin
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, -tvModel.center());
        }

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

        shader.setMat4("u_projection", state.camera.getProjectionMat(window.aspectRatio()));
        shader.setMat4("u_view", state.camera.getViewMat());
        shader.setMat4("u_model", model);
        shader.setMat3("u_normalMatrix", normalMatrix);

        shader.setInt("u_renderMode", state.render_mode);
        shader.setVec3("u_lightPos", state.light_pos);
        shader.setVec3("u_lightColor", state.light_color);
        shader.setVec3("u_viewPos", state.camera.position());
        shader.setVec3("u_objectColor", state.object_color);
        shader.setFloat("u_ambientStrength", state.ambient_strength);
        shader.setFloat("u_specularStrength", state.specular_strength);
        shader.setFloat("u_shininess", state.shininess);

        shader.setBool("u_useMaterialColors", state.use_material_colors);
        shader.setBool("u_useTexture", state.use_texture);
        if (state.use_texture) {
            checkerTexture.bind(0);
            shader.setInt("u_diffuseMap", 0);
        }

        if (state.current_model_idx == 0) {
            monkeyModel.draw(GL_TRIANGLES);
        } else if (state.current_model_idx == 1) {
            dragonModel.draw(GL_TRIANGLES);
        } else {
            tvModel.draw(GL_TRIANGLES);
        }

        // Render ImGui over OpenGL content
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
        state.timer.endFrame();
    }

    // Cleanup Dear ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
