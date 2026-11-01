#include "VulkanSwapchain.hpp"
#include "VkLog.hpp"
#include "VulkanDevice.hpp"


#include <cstdint>
#include <limits> 
#include <algorithm>

VulkanSwapchain::VulkanSwapchain(VulkanDevice& dev, 
                                VkSurfaceKHR s, int w, int h) : 
                                 m_device(dev), surface(s), width(w), height(h)
{

    m_suport = vkutil::QuerySwapChainSupport(m_device.getPhyD(), surface);

    choose_swap_surface_format(m_suport.formats, SwapchainColorMode::SRGB);
    choose_swap_present_mode(m_suport.presentModes);
    choose_swap_extent(m_suport.capabilities);

}


VulkanSwapchain::~VulkanSwapchain() = default;

void VulkanSwapchain::destroy()
{
    
    destroy_swapchain_image_views();

    if (handle != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device.get(), handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
}



void VulkanSwapchain::createSwapChain()
{


    uint32_t imageCount = m_suport.capabilities.minImageCount + 1;

    if (m_suport.capabilities.maxImageCount > 0 && 
        imageCount > m_suport.capabilities.maxImageCount)
    {
        imageCount = m_suport.capabilities.maxImageCount;
    } 

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = surface;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = m_format.format;
    createInfo.imageColorSpace  = m_format.colorSpace;
    createInfo.imageExtent      = m_extent2D;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //----need to consider queue families!!!!!!!!!!!!!!!!!!!!!!!!!
    createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; 
    createInfo.pQueueFamilyIndices   = nullptr;

    createInfo.preTransform   = m_suport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = m_present_mode;
    createInfo.clipped        = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_ASSERT_MSG(vkCreateSwapchainKHR(m_device.get(), &createInfo, nullptr, &handle),
    "failed to create swap chain!");   


    vkGetSwapchainImagesKHR(m_device.get(), handle, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device.get(), handle, &imageCount, swapChainImages.data());
}

void VulkanSwapchain::recreate_swapchain()
{

}

void VulkanSwapchain::create_swapchain_image_views()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) 
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        VK_ASSERT_MSG(vkCreateImageView(m_device.get(), &createInfo, nullptr, &swapChainImageViews[i]),"failed to create image views!"); 

    }
}

void VulkanSwapchain::destroy_swapchain_image_views()
{
    
    for (auto imageView : swapChainImageViews) {

        vkDestroyImageView(m_device.get(), imageView, nullptr);
        
    }
    swapChainImageViews.clear();
}



void VulkanSwapchain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats, SwapchainColorMode colorMode )
{
    if (formats.empty()) 
        LLOGE("No surface formats available");

    if(formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        switch (colorMode) 
        {
            case SwapchainColorMode::SRGB:
                swapChainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;
                m_format = { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
            case SwapchainColorMode::UNORM:
                swapChainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
                m_format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
            case SwapchainColorMode::HDR10:
                swapChainImageFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                m_format = { VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT };
            case SwapchainColorMode::DontCare:
            default:
                swapChainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;
                m_format = { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }
    }

    std::vector<VkFormat> priority;

    switch (colorMode) {
        case SwapchainColorMode::SRGB:
            priority = 
            { 
                VK_FORMAT_B8G8R8A8_SRGB, 
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_FORMAT_B8G8R8A8_UNORM, 
                VK_FORMAT_R8G8B8A8_UNORM 
            };
            break;
        case SwapchainColorMode::UNORM:
            priority = 
            { 
                VK_FORMAT_B8G8R8A8_UNORM, 
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_FORMAT_B8G8R8A8_SRGB, 
                VK_FORMAT_R8G8B8A8_SRGB 
            };
            break;
        case SwapchainColorMode::HDR10: // probably not going to ever use this
            priority = 
            { 
                VK_FORMAT_A2B10G10R10_UNORM_PACK32,
                VK_FORMAT_A2R10G10B10_UNORM_PACK32,
                VK_FORMAT_R16G16B16A16_SFLOAT 
            };
            break;
        case SwapchainColorMode::DontCare:
        default:
            priority = 
            { 
                VK_FORMAT_B8G8R8A8_SRGB, 
                VK_FORMAT_B8G8R8A8_UNORM 
            };
            break;
    }

    for (VkFormat wanted : priority)
    {
        for (const auto& available : formats)
        {
            if (available.format != wanted)
                continue;

            if (colorMode == SwapchainColorMode::HDR10)
            {
                if (available.colorSpace != VK_COLOR_SPACE_HDR10_ST2084_EXT)
                    continue;
            }
            else
            {
                if (available.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    continue;
            }

            m_format = available;
            swapChainImageFormat = m_format.format;
            return;
        }
    }

    swapChainImageFormat = formats[0].format;
    m_format = formats[0];
}


void VulkanSwapchain::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{

    std::vector<VkPresentModeKHR > preffered = 
    {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        VK_PRESENT_MODE_FIFO_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR
    };

    for(VkPresentModeKHR mode : preffered)
        for(VkPresentModeKHR available : availablePresentModes)
        {       
            if(available == mode)
            {
                m_present_mode = mode;
                return;
            }
        }

    m_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    
}

void VulkanSwapchain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) 
    {
        m_extent2D = capabilities.currentExtent;
    } 
    else 
    {
        VkExtent2D actualExtent = 
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        m_extent2D = actualExtent;
    }
}




VkSwapchainKHR VulkanSwapchain::get_handle() const{ return handle; }
VkFormat VulkanSwapchain::get_image_format() const { return swapChainImageFormat; }
VkExtent2D VulkanSwapchain::get_extent() const { return m_extent2D; }

int VulkanSwapchain::get_width() const { return width; }
int VulkanSwapchain::get_height() const { return height; }

std::vector<VkImageView> VulkanSwapchain::get_image_views() { return swapChainImageViews; }
std::vector<VkImage> VulkanSwapchain::get_images() { return swapChainImages; }


void VulkanSwapchain::update_resolution(int w, int h)
{
    width = w;
    height = h;
}
