#pragma once

#include "vulkan/vulkan.h"
#include "Renderer.hpp"

#include <vector>



class VkRenderer : public Renderer
{
public:
    void Initialize(std::vector<const char*>) override;
    void RenderFrame() override;
    void Shutdown() override;

private:
    //VulkanDevice device;
    //VulkanSwapchain swapchain;
    //VulkanPipeline pipeline;
    //VulkanCommands commands;
    //VulkanSync sync;
};