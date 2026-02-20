
#pragma once 

#include <vector>

struct RENDER_INFO
{
    void* windowHandle;
    std::vector<const char*> requiredExtensions;
};


class Renderer
{
public:

    virtual void Initialize(std::vector<const char*>) = 0;
    virtual void RenderFrame() = 0;
    virtual void Shutdown() = 0;

};