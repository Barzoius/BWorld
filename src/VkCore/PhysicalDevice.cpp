#include "PhysicalDevice.hpp"
#include <iostream>
#include <vector>

PhysicalDevice::PhysicalDevice(const Instance& i) : instance(i)
{
    std::cout << "PhysicalDevice constructor\n";
}

PhysicalDevice::~PhysicalDevice()
{

}

void PhysicalDevice::Initialize()
{
    std::cout << "PhysicalDevice Initialize\n";
    pickDevice(); 
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
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
}


QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) 
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) 
        {
            break;
        }

        i++;
    }
    
    return indices;
}

// VkPhysicalDeviceFeatures PhysicalDevice::getFeatures()
// {
//     return;
// }

VkPhysicalDevice PhysicalDevice::getHandle() const
{
    return handle;
}
