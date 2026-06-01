#pragma once

namespace modeye
{
    class timer
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
        timer() = default;
        timer(float target_fps);
        ~timer() = default;

        float time() const;
        float delta_time() const;
        int fps() const;

        void startFrame();
        void endFrame();
    };
}
