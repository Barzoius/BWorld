#include "Engine.hpp"
#include <iostream>
#include "GLFW/glfw3.h"



Window* Engine::window = nullptr;
Renderer* Engine::renderer = nullptr;


void Engine::Init(const WND_SPECS &specs)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    window = new Window(specs);
}

std::vector<const char*> Engine::GetRequiredVulkanExtensions()
{
    uint32_t count = 0;
    const char** glfwExt = glfwGetRequiredInstanceExtensions(&count);

    return std::vector<const char*>(glfwExt, glfwExt + count);
}

Window* Engine::GetWindow()
{
    return window;
}