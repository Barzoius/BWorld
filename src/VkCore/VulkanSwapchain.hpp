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

class VulkanDevice;

class VulkanSwapchain
{
private:
    using VK_SC_SUPPORT = vkutil::SwapChainSupportDetails;
public:
    VulkanSwapchain(VulkanDevice&, VkSurfaceKHR, int, int);
    ~VulkanSwapchain();

    void createSwapChain(); 
    void destroy();
    void create_swapchain_image_views();
    void destroy_swapchain_image_views();
    void update_resolution(int, int);
    void recreate_swapchain();
    
    [[nodiscard]] VkSwapchainKHR get_handle()       const;
    [[nodiscard]] VkFormat       get_image_format() const;
    [[nodiscard]] VkExtent2D     get_extent()       const;

    [[nodiscard]] int get_width()  const;
    [[nodiscard]] int get_height() const;

    [[nodiscard]] std::vector<VkImageView> get_image_views();
    [[nodiscard]] std::vector<VkImage>     get_images();

private:
    void   choose_swap_present_mode(const std::vector<VkPresentModeKHR>&);
    void         choose_swap_extent(const VkSurfaceCapabilitiesKHR&) ;
    void choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>&, 
                                                        SwapchainColorMode);
private:
    VulkanDevice &           m_device;

    VkSurfaceKHR             surface;
    VkSwapchainKHR           handle{};  

    VK_SC_SUPPORT    m_suport;
    VkSurfaceFormatKHR  m_format;
    VkExtent2D       m_extent2D;
    VkPresentModeKHR m_present_mode;

    int                      width;
    int                      height;
    std::vector<VkImage>     swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat                 swapChainImageFormat;

};