#pragma once
#include <string>
#include "GLFW/glfw3.h"

#include "Inputs/Keyboard.hpp"
#include "Inputs/Mouse.hpp"


struct WND_SPECS
{
    std::string wnd_name;
    int wnd_width;
    int wnd_height;

    bool wnd_resizable;
    bool wnd_fullScreen;
    bool wnd_centered;
    bool wnd_vSync;
};

struct WND_DATA {
    GLFWwindow* glfwHandle = nullptr;
};

class Window
{
friend class Engine;
public:
    Window(WND_SPECS specs);
    ~Window();

    void full_screen();

    int should_close() const;
    void close() const;

    void poll_events() const;

    void update_resolution(int, int);
    bool was_resized() const;
    void clear_resize_flag();
    void set_resize_flag();
    

    GLFWwindow* GetHandle() const { return window.glfwHandle; }


private:
    void set_callbacks();
private:
    static void s_framebuffer_resize_callback(GLFWwindow*, int, int);
    static void s_keyboard_callback(GLFWwindow*, int, int, int, int);
    static void s_mouse_move_callback(GLFWwindow*, double, double);
    static void s_mouse_buttons_callback(GLFWwindow*, int, int, int);
    static void s_mouse_wheel_callback(GLFWwindow*, double, double);
    static void s_mouse_enter_callback(GLFWwindow*, int);

public:
    WND_SPECS specs;
    WND_DATA window;

    bool framebufferResized = false;

public:
    Keyboard keyboard;
    Mouse mouse;
};