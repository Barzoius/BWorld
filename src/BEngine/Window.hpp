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
public:
    Window(WND_SPECS specs);
    ~Window();

    void FullScreen();

    int ShouldClose() const;
    void Close() const;

    void PoolEvents() const;

    GLFWwindow* GetHandle() const { return window.glfwHandle; }

    void ProcessInput();

private:
    WND_SPECS specs;
    WND_DATA window;

public:
    Keyboard keyboard;
    Mouse mouse;
};