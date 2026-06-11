#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>


namespace vkutil
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> s_graphics; // graphics + present
        std::optional<uint32_t> s_compute; // async compute
        std::optional<uint32_t> s_transfer; 

    
        bool is_complete() const {
            return s_graphics.has_value();
        }

    };

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