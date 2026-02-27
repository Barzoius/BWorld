#pragma once
#include "Window.hpp"
#include <vector>
#include "Renderer.hpp"



class Engine
{
 public:
    static void Init(const WND_SPECS &);

    static Window* GetWindow();

    static std::vector<const char*>GetRequiredVulkanExtensions();

    static Renderer*& GetRenderer() { return renderer; }

    static double GetElapsedTime();

    static void Exit();

    static SurfaceInfo GetSurfaceInfo();

private:
    static Window* window;
public:
    static Renderer* renderer;
 
};