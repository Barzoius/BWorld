#include "Window.hpp"
#include "GLFW/glfw3.h"

struct OPAQUE_WND {
    GLFWwindow* glfwHandle;
};

Window::Window(WND_SPECS specs) : specs(specs)
{
    window = new OPAQUE_WND();
    window->glfwHandle = glfwCreateWindow(specs.wnd_width, 
                                          specs.wnd_height, 
                                          "world_1", 
                                          nullptr, 
                                          nullptr);
    //if (!window->glfwWindow) throw std::runtime_error("Failed to create window");
}


int Window::ShouldClose() const
{
    return glfwWindowShouldClose(window->glfwHandle);
}


void Window::PoolEvents() const
{
    glfwPollEvents();
}

Window::~Window()
{
    glfwDestroyWindow(window->glfwHandle);
    delete window;
}
