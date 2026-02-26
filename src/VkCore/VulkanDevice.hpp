#pragma once

#include "vulkan/vulkan.h"

#include "Instance.hpp"
#include "VulkanQueue.hpp"

#include <optional>

class VulkanDevice
{
public:
    VulkanDevice(const Instance&);
    ~VulkanDevice();

    void Initialize();

    std::shared_ptr<VulkanQueue> graphicsQueue;
    std::shared_ptr<VulkanQueue> computeQueue;
    //std::shared_ptr<VulkanQueue> transferQueue;

private:

    void pickDevice();
    void createLogicalDevice();
    void createQueue();
    void createSurface();
    
    void findQueueFamilies();
    bool isDeviceSuitable(VkPhysicalDevice);
    void findQueueFamilies(VkPhysicalDevice);
    

private:
    const Instance& instance;
    
    VkPhysicalDevice phyD{};
    VkDevice handle{};
    VkSurfaceKHR surface{};

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;
        

        bool isComplete() const {
            return graphicsFamily.has_value() &&
                //    presentFamily.has_value() &&
                   computeFamily.has_value();
        }
    } indices;
};