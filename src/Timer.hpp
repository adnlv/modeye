#pragma once

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
    Timer(double targetFPS);
    ~Timer() = default;

    double time() const;
    double deltaTime() const;
    int fps() const;
    
    void startFrame();
    void endFrame();
};
