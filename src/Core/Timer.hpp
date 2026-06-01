#pragma once

namespace Modeye
{
    class Timer
    {
    private:
        double m_deltaTime = 0;
        double m_frameStart = 0;

        double m_targetFps = 0;
        double m_targetFrameTime = 0;

        double m_fpsTimer = 0;
        int m_frameCount = 0;
        int m_currentFps = 0;

    public:
        Timer() = default;
        Timer(float targetFps);
        ~Timer() = default;

        float time() const;
        float deltaTime() const;
        int fps() const;

        void startFrame();
        void endFrame();
    };
}
