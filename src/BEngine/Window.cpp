#include "Window.hpp"
#include <stdexcept>

Window::Window(WND_SPECS specs) : specs(specs)
{
    window.glfwHandle = glfwCreateWindow(
        specs.wnd_width,
        specs.wnd_height,
        specs.wnd_name.c_str(),
        nullptr,
        nullptr
    );

    if (!window.glfwHandle)
        throw std::runtime_error("Failed to create window");
}

int Window::ShouldClose() const
{
    return glfwWindowShouldClose(window.glfwHandle);
}

void Window::Close() const
{
    glfwSetWindowShouldClose(window.glfwHandle, true);
}


void Window::PoolEvents() const
{
    glfwPollEvents();
}

Window::~Window()
{
    if (window.glfwHandle)
        glfwDestroyWindow(window.glfwHandle);
}

void Window::ProcessInput()
{    
    keyboard.SetKey(GLFW_KEY_ESCAPE, glfwGetKey(window.glfwHandle, GLFW_KEY_ESCAPE));
}