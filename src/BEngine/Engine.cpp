#include "Engine.hpp"
#include <iostream>

#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h> // Win32-specific functions
#elif defined(__linux__)
    #ifdef USE_X11
        #define GLFW_EXPOSE_NATIVE_X11
        #include <GLFW/glfw3native.h> // X11-specific functions
    #else // Wayland
        #define GLFW_EXPOSE_NATIVE_WAYLAND
        #include <GLFW/glfw3native.h> // Wayland-specific functions
    #endif
#endif



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

void Engine::Exit()
{
    renderer->Shutdown();
}


SurfaceInfo Engine::GetSurfaceInfo()
{
    SurfaceInfo surfaceInfo;

    #ifdef _WIN32
        surfaceInfo.hwnd = glfwGetWin32Window(window->GetHandle());
    #elif defined(__linux__)
    #ifdef USE_X11
        surfaceInfo.display = glfwGetX11Display();
        surfaceInfo.window  = reinterpret_cast<void*>(glfwGetX11Window(window));
    #else // Wayland
        surfaceInfo.display = glfwGetWaylandDisplay();
        surfaceInfo.window  = glfwGetWaylandWindow(window);
    #endif
    #endif

    return surfaceInfo;
}