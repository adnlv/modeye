#include "../Log.hpp"
#include "Window.hpp"

Modeye::Gfx::Window::Window(int width, int height, const std::string& title, GLFWmonitor* monitor)
    : m_glfwMonitor(monitor),
    m_frameBufferWidth(width),
    m_frameBufferHeight(height),
    m_aspectRatio(static_cast<float>(width) / height)
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        Modeye::Log::error("GLFW initialization failed");
        std::abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    m_glfwWindow = glfwCreateWindow(
        m_frameBufferWidth,
        m_frameBufferHeight,
        title.c_str(),
        monitor,
        nullptr);
    if (m_glfwWindow == nullptr)
    {
        Modeye::Log::error("GLFW window creation failed");
        std::abort();
    }

    Modeye::Log::trace("Created {}x{} GLFW window called '{}'", width, height, title);

    glfwMakeContextCurrent(m_glfwWindow);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        Modeye::Log::error("GLAD failed to load OpenGL");
        std::abort();
    }

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    printSystemInfo();

    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwSetFramebufferSizeCallback(m_glfwWindow, glfwFramebufferSizeCallback);
}

Modeye::Gfx::Window::~Window()
{
    if (m_glfwWindow != nullptr)
    {
        Modeye::Log::trace("Destroyed GLFW window");

        glfwDestroyWindow(m_glfwWindow);
        glfwTerminate();
    }
}

void Modeye::Gfx::Window::glfwErrorCallback(int errorCode, const char* description)
{
    Modeye::Log::error("GLFW (errorCode: {}): {}", errorCode, description);
}

void Modeye::Gfx::Window::glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
    auto window = static_cast<Modeye::Gfx::Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (window == nullptr)
    {
        return;
    }

    glViewport(0, 0, width, height);
    window->m_frameBufferWidth = width;
    window->m_frameBufferHeight = height;
    window->m_aspectRatio = static_cast<float>(width) / height;

    Modeye::Log::info("Framebuffer resized to: {}x{} | Aspect Ratio: {:.3f}", width, height, window->m_aspectRatio);
}

void Modeye::Gfx::Window::printSystemInfo()
{
    Modeye::Log::info("OpenGL context created:");
    Modeye::Log::info("  GPU vendor:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    Modeye::Log::info("  GPU renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    Modeye::Log::info("  GL version:   {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Modeye::Log::info("  GLSL version: {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    GLint maxTextureSize = 0;
    GLint maxMsaaSamples = 0;
    GLint maxVertexAttribs = 0;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_SAMPLES, &maxMsaaSamples);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    Modeye::Log::info("GPU hardware capability limits:");
    Modeye::Log::info("  Max 2D texture size:   {}x{}", maxTextureSize, maxTextureSize);
    Modeye::Log::info("  Max MSAA samples:      {}x", maxMsaaSamples);
    Modeye::Log::info("  Max vertex attributes: {}", maxVertexAttribs);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (primaryMonitor)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        Modeye::Log::info("Primary monitor detected: {}x{} @ {}Hz", mode->width, mode->height, mode->refreshRate);
    }
}

GLFWwindow* Modeye::Gfx::Window::glfwWindow() const
{
    return m_glfwWindow;
}

GLFWmonitor* Modeye::Gfx::Window::glfwMonitor() const
{
    return m_glfwMonitor;
}

int Modeye::Gfx::Window::frameBufferWidth() const
{
    return m_frameBufferWidth;
}

int Modeye::Gfx::Window::frameBufferHeight() const
{
    return m_frameBufferHeight;
}

float Modeye::Gfx::Window::aspectRatio() const
{
    return m_aspectRatio;
}

bool Modeye::Gfx::Window::shouldClose() const
{
    return glfwWindowShouldClose(m_glfwWindow);
}

void Modeye::Gfx::Window::pollEvents() const
{
    glfwPollEvents();
}

void Modeye::Gfx::Window::swapBuffers() const
{
    glfwSwapBuffers(m_glfwWindow);
}

int Modeye::Gfx::Window::getKey(int key)
{
    return glfwGetKey(m_glfwWindow, key);
}

void Modeye::Gfx::Window::setInputMode(int mode, int value)
{
    glfwSetInputMode(m_glfwWindow, mode, value);
}

void Modeye::Gfx::Window::setKeyCallback(GLFWkeyfun callback)
{
    glfwSetKeyCallback(m_glfwWindow, callback);
}

void Modeye::Gfx::Window::setScrollCallback(GLFWscrollfun callback)
{
    glfwSetScrollCallback(m_glfwWindow, callback);
}

void Modeye::Gfx::Window::setCursorPosCallback(GLFWcursorposfun callback)
{
    glfwSetCursorPosCallback(m_glfwWindow, callback);
}
