#pragma once

#include "vulkan/vulkan.h"

#include "VkContext.hpp"
#include "VkContexts/VkSwapChainContext.hpp"

class RenderPass
{
public:
    RenderPass(VkContext&, const VkSwapchainContext&);
    ~RenderPass();
    void Destroy();

    void createRenderPass();
    VkRenderPass getRenderPass() const;


    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

private:
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkContext& context;
    VkSwapchainContext swapchainContext;
};