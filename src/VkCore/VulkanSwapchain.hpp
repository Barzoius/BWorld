#pragma once
#include "vulkan/vulkan.h"
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapchain
{

public:
    VulkanSwapchain(const VkPhysicalDevice&, const VkSurfaceKHR&);
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);

private:

private:
    const VkPhysicalDevice& phyD;
    const VkSurfaceKHR& surface;
    VkSwapchainKHR swapChain{};

    
};