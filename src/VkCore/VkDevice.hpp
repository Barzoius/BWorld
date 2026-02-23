#pragma once

#include "vulkan/vulkan.h"
//#include "PhysicalDevice.hpp"

class VulkanDevice
{
public:
    void Initialize();
    void Cleanup();

private:
    //const PhysicalDevice& physicalDevice;
    VkDevice handle{};

};