#include "VulkanSwapchain.hpp"
#include <iostream>

VulkanSwapchain::VulkanSwapchain(const VkPhysicalDevice& pd, 
                                 const VkSurfaceKHR& s) : phyD(pd), surface(s)
{
}

SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surf) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surf, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surf, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surf, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surf, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surf, 
                                                  &presentModeCount, details.presentModes.data());
    }

    return details;
}