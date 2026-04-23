#pragma once
#include "vulkan/vulkan.h"
#include <vector>

#include "VkUtils.hpp"

enum class SwapchainColorMode {
    SRGB,        
    UNORM,      
    HDR10,          
    DontCare
};

class VulkanSwapchain
{
public:
    VulkanSwapchain(VkDevice, VkPhysicalDevice, VkSurfaceKHR, int, int);
    ~VulkanSwapchain();

    void createSwapChain(); 
    void Destroy();
    void createSwapChainImageViews();
    void destroySwapChainImageViews();

    void recreateSwapchain();
    
    VkSwapchainKHR getHandle() const;

    VkFormat getImageFormat() const;
    VkExtent2D getExtent() const;

    int getWidth() const;
    int getHeight() const;


private:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&, SwapchainColorMode);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&) ;
private:
    VkDevice device;
    VkPhysicalDevice phyD;
    VkSurfaceKHR surface;
    VkSwapchainKHR handle{};  

    int width;
    int height;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};