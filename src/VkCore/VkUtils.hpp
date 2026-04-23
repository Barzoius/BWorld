#pragma once

#include <vulkan/vulkan.h>
#include <vector>



namespace vkutil
{
    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(
        VkPhysicalDevice device,
        VkSurfaceKHR surface);
}