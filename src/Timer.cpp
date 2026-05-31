#include "Log.hpp"
#include "Timer.hpp"
#include <GLFW/glfw3.h>

Timer::Timer(double targetFPS)
{
    if (targetFPS == 0)
    {
        return;
    }

    m_targetFPS = targetFPS;
    m_targetFrameTime = 1 / m_targetFPS;
}

double Timer::time() const
{
    return m_frameStart;
}

double Timer::deltaTime() const
{
    return m_deltaTime;
}

int Timer::fps() const
{
    return m_currentFPS;
}

void Timer::startFrame()
{
    m_frameStart = glfwGetTime();
}

void Timer::endFrame()
{
    const double frameTime = glfwGetTime() - m_frameStart;

    if (m_targetFrameTime > 0 && frameTime < m_targetFrameTime)
    {
        // Busy-wait for the remaining precise milliseconds
        while (glfwGetTime() - m_frameStart < m_targetFrameTime)
        {
        }
    }

    m_deltaTime = glfwGetTime() - m_frameStart;

    m_frameCount++;
    m_fpsTimer += m_deltaTime;

    if (m_fpsTimer >= 1)
    {
        m_currentFPS = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer = 0;

        Log::debug("framesPerSecond {:>5} | deltaTime {:.7f}", m_currentFPS, m_deltaTime);
    }
}

