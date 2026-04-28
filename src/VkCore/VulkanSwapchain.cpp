#include "VulkanSwapchain.hpp"
#include <iostream>

#include <cstdint>
#include <limits> 
#include <algorithm>

VulkanSwapchain::VulkanSwapchain(VkDevice d,
                                 VkPhysicalDevice pd, 
                                VkSurfaceKHR s, int w, int h) : 
                                 device(d), phyD(pd), surface(s), width(w), height(h)
{
    std::cout << "Device handle: " << device << "\n";
}

VulkanSwapchain::~VulkanSwapchain() = default;

void VulkanSwapchain::Destroy()
{
    
    destroySwapChainImageViews();

    if (handle != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
    

    std::cout << "Swapchain Destroyed\n";
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats, SwapchainColorMode colorMode )
{
    if (formats.empty()) 
        throw std::runtime_error("No surface formats available");

    if(formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        switch (colorMode) 
        {
            case SwapchainColorMode::SRGB:
                swapChainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;
                return { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
            case SwapchainColorMode::UNORM:
                swapChainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
                return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
            case SwapchainColorMode::HDR10:
                swapChainImageFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                return { VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT };
            case SwapchainColorMode::DontCare:
            default:
                swapChainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;
                return { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
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
            if (available.format == wanted)
            {
                if(colorMode == SwapchainColorMode::HDR10)
                {
                    if (available.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
                    {
                        swapChainImageFormat = available.format;
                        return available;

                    }
                }
                else
                {
                    if (available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    {
                        swapChainImageFormat = available.format;
                        return available;
                    }
                }
            }
        }
    }

    swapChainImageFormat = formats[0].format;
    return formats[0];
}


VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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
                return mode;
            }
        }

    return VK_PRESENT_MODE_FIFO_KHR;
    
}

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        swapChainExtent = capabilities.currentExtent;
        return capabilities.currentExtent;
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

        swapChainExtent = actualExtent;
        return actualExtent;
    }
} 

void VulkanSwapchain::createSwapChain()
{
    vkutil::SwapChainSupportDetails swapChainSupport = vkutil::QuerySwapChainSupport(phyD, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, SwapchainColorMode::SRGB);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && 
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    } 

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //----need to consider queue families!!!!!!!!!!!!!!!!!!!!!!!!!
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; 
    createInfo.pQueueFamilyIndices = nullptr;

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &handle) != VK_SUCCESS)   
        throw std::runtime_error("failed to create swap chain!");

    std::cout << "Swapchain created\n";

    vkGetSwapchainImagesKHR(device, handle, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, handle, &imageCount, swapChainImages.data());
}

void VulkanSwapchain::createSwapChainImageViews()
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

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) 
            throw std::runtime_error("failed to create image views!");

    }
}

void VulkanSwapchain::destroySwapChainImageViews()
{
    
    for (auto imageView : swapChainImageViews) {

        vkDestroyImageView(device, imageView, nullptr);
        
    }
    swapChainImageViews.clear();
    std::cout << "SwapChainImageViews Destroyed \n";

}

VkSwapchainKHR VulkanSwapchain::getHandle() const
{
    return handle;
}


VkFormat VulkanSwapchain::getImageFormat() const
{
    return swapChainImageFormat;
}


VkExtent2D VulkanSwapchain::getExtent() const
{
    return swapChainExtent;
}

int VulkanSwapchain::getWidth() const
{
    return width;
}
int VulkanSwapchain::getHeight() const
{
    return height;
}


std::vector<VkImageView> VulkanSwapchain::getImageViews()
{
    return swapChainImageViews;
}
