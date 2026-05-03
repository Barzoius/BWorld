#include "Window.hpp"
#include <stdexcept>

#include <iostream>

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

    set_callbacks();
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


void Window::set_callbacks()
{
    glfwSetWindowUserPointer(window.glfwHandle, this);

    glfwSetKeyCallback(window.glfwHandle, s_keyboard_callback);

    glfwSetCursorPosCallback(window.glfwHandle, s_mouse_move_callback);
    glfwSetScrollCallback(window.glfwHandle, s_mouse_wheel_callback);
    glfwSetCursorEnterCallback(window.glfwHandle, s_mouse_enter_callback);
}

void Window::s_keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win) return;

    Keyboard& kbd = win->keyboard;

    if(action == GLFW_PRESS)
        kbd.OnKeyPressed(key);
    else if(action == GLFW_RELEASE)
        kbd.OnKeyReleased(key);
    else if(action == GLFW_REPEAT && kbd.AutorepeatIsEnabled())
        return; // not implemented yet
}

void Window::s_mouse_move_callback(GLFWwindow* window, double x_pos, double y_pos)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win) return;

    Mouse& mouse = win->mouse;

    mouse.on_mouse_move(static_cast<int>(x_pos),
                        static_cast<int>(y_pos));

}

void Window::s_mouse_buttons_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win) return;
    
    auto [x, y] = win->mouse.get_pos();

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
            win->mouse.on_left_press(x, y);
        else if (action == GLFW_RELEASE)
            win->mouse.on_left_release(x, y);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
            win->mouse.on_right_press(x, y);
        else if (action == GLFW_RELEASE)
            win->mouse.on_right_release(x, y);
    }

}

void Window::s_mouse_wheel_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win) return;

    auto [x, y] = win -> mouse.get_pos();
    win -> mouse.on_wheel_delta(x, y, static_cast<int>(y_offset * 120)); // this has to go
    
}

void Window::s_mouse_enter_callback(GLFWwindow* window, int entered)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win) return;

    if(entered) win -> mouse.on_mouse_enter();
    else win -> mouse.on_mouse_leave();
}