#pragma once
#include "vulkan/vulkan.h"
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

enum class SwapchainColorMode {
    SRGB,        
    UNORM,      
    HDR10,          
    DontCare
};

class VulkanSwapchain
{
public:
    VulkanSwapchain(const VkDevice&, const VkPhysicalDevice&, const VkSurfaceKHR&, int, int);
    ~VulkanSwapchain();
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);

    void createSwapChain(); 
    void recreateSwapchain();

    VkSwapchainKHR getHandle() const;

private:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&, SwapchainColorMode);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&) ;
private:
    const VkDevice& device;
    const VkPhysicalDevice& phyD;
    const VkSurfaceKHR& surface;
    VkSwapchainKHR handle{};  

    int width;
    int height;

    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};