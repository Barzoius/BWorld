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
    void create_swapchain_image_views();
    void destroy_swapchain_image_views();

    void update_resolution(int, int);

    void recreate_swapchain();
    
    VkSwapchainKHR get_handle() const;

    VkFormat get_image_format() const;
    VkExtent2D get_extent() const;

    int get_width() const;
    int get_height() const;

    std::vector<VkImageView> get_image_views();


private:
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>&, SwapchainColorMode);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR&) ;
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