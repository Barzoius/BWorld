#pragma once

#include <chrono>

class Timer
{
public:
    Timer();
    float mark()       noexcept;
    float peek() const noexcept;
private:
    std::chrono::steady_clock::time_point last;
};

inline Timer::Timer() { last = std::chrono::steady_clock::now(); }

inline float Timer::mark() noexcept
{
    const auto old = last;
    last = std::chrono::steady_clock::now();
    const std::chrono::duration<float> frameTime = last - old;
    return frameTime.count();

}

inline float Timer::peek() const noexcept
{
    return std::chrono::duration<float>(
           std::chrono::steady_clock::now() - last).count();
}