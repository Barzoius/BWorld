#pragma once
#include <string>

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

struct OPAQUE_WND;
typedef OPAQUE_WND *WND_DATA;

class Window
{
public:
    Window(WND_SPECS);
    ~Window();

    void FullScreen();

    int ShouldClose() const;

    void PoolEvents() const;

private:
    WND_SPECS specs;
    WND_DATA window;
};