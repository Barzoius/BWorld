#pragma once 

#include "SurfaceInfo.hpp"
#include <vector>

class Renderer
{
public:

    virtual void Initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&) = 0;
    virtual void RenderFrame() = 0;
    virtual void Shutdown() = 0;

    virtual void UpdateResolution(const Resolution&) = 0;
};