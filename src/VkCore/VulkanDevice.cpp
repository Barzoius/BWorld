#include "VulkanDevice.hpp"

#include <set>

VulkanDevice::VulkanDevice(const Instance& i) : instance(i)
{
    std::cout << "VulkanDevice constructor\n";
}

VulkanDevice::~VulkanDevice()
{
    graphicsQueue.reset();
    computeQueue.reset();

    vkDestroySwapchainKHR(handle, swapchain->getHandle(), nullptr);

    if(handle != VK_NULL_HANDLE) 
    {
        vkDeviceWaitIdle(handle);
        vkDestroyDevice(handle, nullptr);
        std::cout << "LogicalDevice destructor\n";
    }
}

void VulkanDevice::Initialize()
{
    pickDevice();
    createLogicalDevice();
    swapchain = std::make_unique<VulkanSwapchain>(handle, phyD, instance.getSurfaceHandle(), 
                                                        instance.getResolution().width, 
                                                        instance.getResolution().height);
    swapchain -> createSwapChain();
}

void VulkanDevice::pickDevice()
{
        std::cout << "Instance handle at pickDevice = "
              << instance.handle << "\n";

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
            phyD = device;
            break;
        }
    }

    if (phyD == VK_NULL_HANDLE) 
    {
        std::cout<< "ERRORRRp\n";
        throw std::runtime_error("failed to find a suitable GPU!");
    }
        

    std::cout << "PhysicalDevice picked\n";
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    findQueueFamilies(device);

    bool swapChainAdequate = false;
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    if(extensionsSupported)
    {
        swapChainAdequate = !swapchain->querySwapChainSupport(device, instance.getSurfaceHandle()).formats.empty() &&
                            !swapchain->querySwapChainSupport(device, instance.getSurfaceHandle()).presentModes.empty();

    }

    return indices.isComplete() && extensionsSupported;

}

void VulkanDevice::findQueueFamilies(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    int i = 0;
    for (const auto& queueFamily : queueFamilies) 
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices.computeFamily = i;
         
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance.getSurfaceHandle(), &presentSupport);
        if (presentSupport) 
            indices.presentFamily = i;
            

        if (indices.isComplete())
            break; 


        i++;
    }
    
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VulkanDevice::createLogicalDevice()
{
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1; 
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(phyD, &createInfo, nullptr, &handle) != VK_SUCCESS) 
        throw std::runtime_error("failed to create logical device!");

    std::cout << "LogicalDevice created\n";
    
}

