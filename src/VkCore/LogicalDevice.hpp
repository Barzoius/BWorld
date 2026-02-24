#pragma once

#include "vulkan/vulkan.h"
#include "PhysicalDevice.hpp"

class LogicalDevice
{
public:
    LogicalDevice(const PhysicalDevice&);
    ~LogicalDevice();
    void Initialize();
    void Cleanup();
private:
    const PhysicalDevice& physicalDevice;
    VkDevice handle{};

};