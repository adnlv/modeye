#pragma once

#include <GLFW/glfw3.h>
#include "Log.hpp"

class Timer
{
private:
    double m_deltaTime = 0;
    double m_frameStart = 0;

    double m_targetFPS = 0;
    double m_targetFrameTime = 0;

    double m_fpsTimer = 0;
    int m_frameCount = 0;
    int m_currentFPS = 0;

public:
    Timer() = default;
    Timer(double targetFPS)
    {
        if (targetFPS == 0)
        {
            return;
        }

        m_targetFPS = targetFPS;
        m_targetFrameTime = 1 / m_targetFPS;
    }
    ~Timer() = default;

    double time() const
    {
        return m_frameStart;
    }

    double deltaTime() const
    {
        return m_deltaTime;
    }

    int fps() const
    {
        return m_currentFPS;
    }

    void startFrame()
    {
        m_frameStart = glfwGetTime();
    }

    void endFrame()
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
};
