#include "../Log.hpp"
#include "Window.hpp"

static void _glDebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::string source_str;
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        source_str = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_str = "WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_str = "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_str = "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "OTHER";
        break;
    }

    std::string type_str;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        type_str = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_str = "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_str = "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        type_str = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "OTHER";
        break;
    }

    std::string severity_str;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_str = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_str = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severity_str = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_str = "NOTIFICATION";
        break;
    }

    Modeye::Log::debug("OpenGL sent {} severity {} from {} ({}): {}", severity_str, type_str, source_str, id, message);
}

Modeye::Gfx::Window::Window(int width, int height, const std::string& title, GLFWmonitor* monitor)
    : m_glfwMonitor(monitor),
    m_frameBufferWidth(width),
    m_frameBufferHeight(height),
    m_aspectRatio(static_cast<float>(width) / height)
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        Modeye::Log::error("GLFW initialization failed");
        std::abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 8);

    m_glfwWindow = glfwCreateWindow(
        m_frameBufferWidth,
        m_frameBufferHeight,
        title.c_str(),
        monitor,
        nullptr);
    if (m_glfwWindow == nullptr) {
        Modeye::Log::error("GLFW window creation failed");
        std::abort();
    }

    Modeye::Log::trace("Created {}x{} GLFW window called '{}'", width, height, title);

    glfwMakeContextCurrent(m_glfwWindow);

    if (!gladLoadGL(glfwGetProcAddress)) {
        Modeye::Log::error("GLAD failed to load OpenGL");
        std::abort();
    }

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    int flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(_glDebugOutputCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    printSystemInfo();

    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwSetFramebufferSizeCallback(m_glfwWindow, glfwFramebufferSizeCallback);
}

Modeye::Gfx::Window::~Window()
{
    if (m_glfwWindow != nullptr) {
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
    if (window == nullptr) {
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
    if (primaryMonitor) {
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        Modeye::Log::info("Primary monitor detected: {}x{} @ {}Hz", mode->width, mode->height, mode->refreshRate);
    }
}

Modeye::Gfx::Window::Window(Window&& other) noexcept
    : m_glfwWindow(other.m_glfwWindow),
    m_glfwMonitor(other.m_glfwMonitor),
    m_frameBufferWidth(other.m_frameBufferWidth),
    m_frameBufferHeight(other.m_frameBufferHeight),
    m_aspectRatio(other.m_aspectRatio)
{
    other.m_glfwWindow = nullptr;
    other.m_glfwMonitor = nullptr;
}

Modeye::Gfx::Window& Modeye::Gfx::Window::operator=(Window&& other) noexcept
{
    if (this != &other) {
        if (m_glfwWindow) {
            glfwDestroyWindow(m_glfwWindow);
        }

        m_glfwWindow = other.m_glfwWindow;
        m_glfwMonitor = other.m_glfwMonitor;
        m_frameBufferWidth = other.m_frameBufferWidth;
        m_frameBufferHeight = other.m_frameBufferHeight;
        m_aspectRatio = other.m_aspectRatio;

        other.m_glfwWindow = nullptr;
        other.m_glfwMonitor = nullptr;
    }
    return *this;
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
