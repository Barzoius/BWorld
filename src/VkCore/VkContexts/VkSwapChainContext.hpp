#pragma once

#include  "vulkan/vulkan.h"
#include "vector"

struct VkSwapchainContext {
    VkSurfaceKHR surface;

    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    VkExtent2D extent;

    int width;
    int height;
};