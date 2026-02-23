#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"

#include <vector>
#include <iostream>

#include "Instance.hpp"

class VkRenderer : public Renderer
{
public:
    void Initialize(std::vector<const char*>) override;
    void RenderFrame() override;
    void Shutdown() override;

private:
    Instance instance;    
   
};
