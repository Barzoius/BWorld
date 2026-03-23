#pragma once

#include "vulkan/vulkan.h"

#include "VkContext.hpp"

class RenderPass
{
public:
    RenderPass(VkContext&);
    ~RenderPass();

    void createRenderPass();
    VkRenderPass get() const;


    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

private:
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkContext& context;
};