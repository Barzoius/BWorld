#pragma once

#include "vulkan/vulkan.h"


#include "Instance.hpp"
#include "VkUtils.hpp"
#include "VulkanQueue.hpp"
#include "VulkanSwapchain.hpp"


class VulkanDevice
{
public:
    VulkanDevice(const Instance&);
    ~VulkanDevice();

    void Initialize();
    void Destroy();

    // std::shared_ptr<VulkanQueue> graphicsQueue;
    // std::shared_ptr<VulkanQueue> computeQueue;
    // std::shared_ptr<VulkanQueue> presentQueue;
    //std::shared_ptr<VulkanQueue> transferQueue;

    VkDevice get() const;
    VkPhysicalDevice getPhyD() const;
    vkutil::QueueFamilyIndices getDeviceIndices() const;

private:

    void pickDevice();
    void createLogicalDevice();
    void createQueue();
    

    bool isDeviceSuitable(VkPhysicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice);
    void findQueueFamilies(VkPhysicalDevice);

    
    
    
private:
    const Instance& instance;    
    VkPhysicalDevice phyD{};
    VkDevice handle{};
    VkSurfaceKHR surface{};


    vkutil::QueueFamilyIndices indices;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME };

public:
    // these have to be moved from here !!!
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};