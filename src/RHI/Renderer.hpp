#pragma once 

#include "SurfaceInfo.hpp"
#include "Context.hpp"
#include <vector>


class Renderer
{
public:
    Renderer(Context& ctx) : context(ctx) {}
    virtual ~Renderer() = default;
    
    virtual void Initialize(Context&) = 0;
    virtual void RenderFrame() = 0;
    virtual void Shutdown() = 0;

    virtual void UpdateResolution(const Resolution&) = 0;

private: 
    Context& context;
};