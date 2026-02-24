#include "LogicalDevice.hpp"

LogicalDevice::LogicalDevice(const PhysicalDevice& pd) : physicalDevice(pd)
{
    std::cout << "LogicalDevice constructor\n";
}

LogicalDevice::~LogicalDevice()
{
    vkDestroyDevice(handle, nullptr);
}

void LogicalDevice::Initialize()
{
    std::cout << "LogicalDevice initialized\n";
    
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    //... didnt finish here...//


    VkPhysicalDeviceFeatures deviceFeatures{}; // should move this into physical device and make it requestable
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1; 
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice.getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) 
        throw std::runtime_error("failed to create logical device!");

}
