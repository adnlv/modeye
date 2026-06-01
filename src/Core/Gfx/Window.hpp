#pragma once
// Do not sort these two headers
#include <glad/gl.h>
// GLAD must be included first
#include <GLFW/glfw3.h>

#include <string>

namespace Modeye::Gfx
{
    class Window
    {
    public:
        Window(int width, int height, const std::string& title, GLFWmonitor* monitor = nullptr);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&& other) noexcept;
        Window& operator=(Window&& other) noexcept;

        GLFWwindow* glfwWindow() const;
        GLFWmonitor* glfwMonitor() const;
        int frameBufferWidth() const;
        int frameBufferHeight() const;
        float aspectRatio() const;

        bool shouldClose() const;
        void pollEvents() const;
        void swapBuffers() const;

        int getKey(int key);
        void setInputMode(int mode, int value);
        void setKeyCallback(GLFWkeyfun callback);
        void setScrollCallback(GLFWscrollfun callback);
        void setCursorPosCallback(GLFWcursorposfun callback);

    private:
        GLFWwindow* m_glfwWindow;
        GLFWmonitor* m_glfwMonitor;

        int m_frameBufferWidth;
        int m_frameBufferHeight;
        float m_aspectRatio;

        static void glfwErrorCallback(int errorCode, const char* description);
        static void glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
        static void printSystemInfo();
    };
}
