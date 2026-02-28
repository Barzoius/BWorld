#pragma once

#include "vulkan/vulkan.h"


#include "Instance.hpp"
#include "VulkanQueue.hpp"
#include "VulkanSwapchain.hpp"

#include <optional>

class VulkanDevice
{
public:
    VulkanDevice(const Instance&);
    ~VulkanDevice();

    void Initialize();

    std::shared_ptr<VulkanQueue> graphicsQueue;
    std::shared_ptr<VulkanQueue> computeQueue;
    std::shared_ptr<VulkanQueue> presentQueue;

    //std::shared_ptr<VulkanQueue> transferQueue;

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

    std::unique_ptr<VulkanSwapchain> swapchain;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;
        

        bool isComplete() const {
            return graphicsFamily.has_value() &&
                   presentFamily.has_value() &&
                   computeFamily.has_value();
        }
    } indices;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};