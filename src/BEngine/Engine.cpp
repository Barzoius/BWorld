#include "Engine.hpp"
#include <iostream>
#include "GLFW/glfw3.h"


Window* Engine::window = nullptr;

Window* Engine::Init(const WND_SPECS &specs)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    window = new Window(specs);

    return window;
}

Window* Engine::GetWindow()
{
    return window;
}