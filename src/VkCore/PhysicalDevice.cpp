#include "PhysicalDevice.hpp"
#include <iostream>
#include <vector>

PhysicalDevice::PhysicalDevice(const Instance& i) : instance(i)
{

}

PhysicalDevice::~PhysicalDevice()
{

}

void PhysicalDevice::pickDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.handle, &deviceCount, nullptr);

    if (deviceCount == 0) 
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.handle, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device)) 
        {
            handle = device;
            break;
        }
    }

    if (handle == VK_NULL_HANDLE) 
        throw std::runtime_error("failed to find a suitable GPU!");


}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    return true;
}