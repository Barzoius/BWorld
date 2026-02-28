#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"

#include <vector>
#include <iostream>

#include "Instance.hpp"
#include "VulkanDevice.hpp"

class VkRenderer : public Renderer
{
public:
    void Initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&) override;
    void RenderFrame() override;
    void Shutdown() override;

    void UpdateResolution(const Resolution&) override;

    VkRenderer();

private:
    Instance instance;
    VulkanDevice device;
    
};
