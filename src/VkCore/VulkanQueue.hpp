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

private:
    VkDevice device;
    uint32_t familyIndex;
    VkQueue handle;

};