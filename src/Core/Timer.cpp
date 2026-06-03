#include "Log.hpp"
#include "Timer.hpp"
#include <GLFW/glfw3.h>

Modeye::Timer::Timer(float targetFps)
{
    if (targetFps == 0) {
        return;
    }

    m_targetFps = targetFps;
    m_targetFrameTime = 1 / targetFps;
}

float Modeye::Timer::time() const
{
    return static_cast<float>(m_frameStart);
}

float Modeye::Timer::deltaTime() const
{
    return static_cast<float>(m_deltaTime);
}

int Modeye::Timer::fps() const
{
    return m_currentFps;
}

void Modeye::Timer::startFrame()
{
    m_frameStart = glfwGetTime();
}

void Modeye::Timer::endFrame()
{
    const double frameTime = glfwGetTime() - m_frameStart;

    if (m_targetFrameTime > 0 && frameTime < m_targetFrameTime) {
        // Busy-wait for the remaining precise milliseconds
        while (glfwGetTime() - m_frameStart < m_targetFrameTime) {
        }
    }

    m_deltaTime = glfwGetTime() - m_frameStart;

    m_frameCount++;
    m_fpsTimer += m_deltaTime;

    if (m_fpsTimer >= 1) {
        m_currentFps = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer = 0;

        Modeye::Log::debug("framesPerSecond {:>5} | deltaTime {:.7f}", m_currentFps, m_deltaTime);
    }
}
