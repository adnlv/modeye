#include "log.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>

modeye::timer::timer(float target_fps)
{
    if (target_fps == 0)
    {
        return;
    }

    m_targetFPS = target_fps;
    m_targetFrameTime = 1 / target_fps;
}

float modeye::timer::time() const
{
    return static_cast<float>(m_frameStart);
}

float modeye::timer::delta_time() const
{
    return static_cast<float>(m_deltaTime);
}

int modeye::timer::fps() const
{
    return m_currentFPS;
}

void modeye::timer::startFrame()
{
    m_frameStart = glfwGetTime();
}

void modeye::timer::endFrame()
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

        modeye::log::debug("framesPerSecond {:>5} | deltaTime {:.7f}", m_currentFPS, m_deltaTime);
    }
}
