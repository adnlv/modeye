#include <cassert>
#include <iostream>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

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

    double time = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        time = glfwGetTime();

        glm::mat4 projection = glm::perspective(glm::radians(45.f), 720.f / 480.f, 0.1f, 100.f);
        glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 mvp = projection * view * model;

        shader.use();
        shader.setFloat("u_time", static_cast<float>(time));

        GLuint matrixId = glGetUniformLocation(shader.id(), "u_mvp");
        glUniformMatrix4fv(matrixId, 1, false, &mvp[0][0]);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
