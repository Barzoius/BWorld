#pragma once
#include "Window.hpp"

class Engine
{
 public:
    static Window* Init(const WND_SPECS &);

    static Window* GetWindow();

    static double GetElapsedTime();

    static void Exit();

 private:
    static Window* window;
};