#pragma once

#include "vulkan/vulkan.h"
#include <vector>

class VulkanQueue
{
public:
    VulkanQueue(VkDevice device, uint32_t familyIndex, uint32_t count, float* priorities);
    
    void Submit();
    void Wait();

    VkQueue getHandle() const;
    void createQueue();

private:
    VkDevice device;
    uint32_t familyIndex;
    VkQueue handle;

    VkDeviceQueueCreateInfo queueCreateInfo{};

};