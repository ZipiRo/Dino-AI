#pragma once

#include <chrono>

class Timer
{
private:
    Timer() { StartTime = std::chrono::steady_clock::now(); }

    std::chrono::steady_clock::time_point StartTime;
    std::chrono::duration<double> DeltaTime;

public:
    static float TimeScale;
    static double deltaTime;

    Timer(const Timer &) = delete;
    void operator=(const Timer &) = delete;

    static Timer &GetInstance();

    static void Tick();
    static void Reset();
};
double Timer::deltaTime = 0.0;
float Timer::TimeScale = 1.0f;

Timer &Timer::GetInstance()
{
    static Timer instance;
    return instance;
}

void Timer::Tick()
{
    auto &instance = GetInstance();
    instance.DeltaTime = std::chrono::steady_clock::now() - instance.StartTime;
    deltaTime = instance.DeltaTime.count() * TimeScale;
}

void Timer::Reset()
{
    auto &instance = GetInstance();
    instance.StartTime = std::chrono::steady_clock::now();
}