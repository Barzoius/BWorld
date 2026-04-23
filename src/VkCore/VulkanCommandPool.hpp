#pragma once 

#include  "vulkan/vulkan.h"
#include "VulkanDevice.hpp"

class VulkanCommandPool
{
public:
    VulkanCommandPool(VulkanDevice&);
    ~VulkanCommandPool();

    void Initialize();

    void initBuffer();
    void recordCommandBuffer(uint32_t imageIndex); 

private:
    VulkanDevice& device;
    VkCommandPool handle{};

    VkCommandBuffer commandBuffer{};
};