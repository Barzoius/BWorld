#pragma once 

#include "SurfaceInfo.hpp"
#include "Context.hpp"

#include "RenderObject.hpp"


class Renderer
{
public:
    Renderer(Context& ctx) : context(ctx) {}
    virtual ~Renderer() = default;
    
    virtual void Initialize(Context&) = 0;
    virtual void RenderFrame() = 0;
    virtual void Shutdown() = 0;
    virtual void Submit(const RenderObject&) = 0;
    //virtual void BatchSubmit() = 0;

    virtual void UpdateResolution(const Resolution&) = 0;

private: 
    Context& context;
};