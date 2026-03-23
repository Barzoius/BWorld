#pragma once

#include "SurfaceInfo.hpp"
#include <vector>

class Context
{
public:
    virtual ~Context() = default;
    virtual void Initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&) = 0;
    virtual void Destroy() = 0;
};