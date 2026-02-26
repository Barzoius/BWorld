#pragma once
#include "Window.hpp"
#include <vector>
#include "Renderer.hpp"

struct VulkanSurfaceInfo {
#ifdef _WIN32
    void* hwnd = nullptr;
#elif defined(__linux__)
    void* display = nullptr; // X11: Display*, Wayland: wl_display*
    void* window  = nullptr; // X11: Window, Wayland: wl_surface*
#endif
};


class Engine
{
 public:
    static void Init(const WND_SPECS &);

    static Window* GetWindow();

    static std::vector<const char*>GetRequiredVulkanExtensions();

    static Renderer*& GetRenderer() { return renderer; }

    static double GetElapsedTime();

    static void Exit();

    static void setVulkanSurfaceInfo();

private:
    static Window* window;
public:
    static Renderer* renderer;

    static VulkanSurfaceInfo vulkanSurfaceInfo;
    
};