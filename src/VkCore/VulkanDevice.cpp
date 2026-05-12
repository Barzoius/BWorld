#include "VulkanDevice.hpp"
#include "VkUtils.hpp"
#include <set>

VulkanDevice::VulkanDevice(const Instance& i) : instance(i)
{
    std::cout << "VulkanDevice constructor\n";
}

VulkanDevice::~VulkanDevice() = default;

void VulkanDevice::Destroy()
{
    if(handle != VK_NULL_HANDLE) 
    {
        vkDeviceWaitIdle(handle);
        vkDestroyDevice(handle, nullptr);
        std::cout << "LogicalDevice detroyed\n";
    }
}

void VulkanDevice::Initialize()
{
    pick_device();
    create_logical_device();
}

VkDevice VulkanDevice::get() const
{
    return handle;
}

VkPhysicalDevice VulkanDevice::getPhyD() const
{
    return phyD;
}


void VulkanDevice::pick_device()
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
        if (is_device_suitable(device)) 
        {
            phyD = device;
            break;
        }
    }

    if (phyD == VK_NULL_HANDLE) 
    {
        std::cout<< "ERRORRR\n";
        throw std::runtime_error("failed to find a suitable GPU!");
    }
        

    std::cout << "PhysicalDevice picked\n";
}

bool VulkanDevice::is_device_suitable(VkPhysicalDevice device)
{
    find_queue_families(device);

    bool swapChainAdequate = false;
    bool extensionsSupported = check_device_extension_support(device);

    if(extensionsSupported)
    {
        swapChainAdequate = !vkutil::QuerySwapChainSupport(device, instance.get_surface_handle()).formats.empty() &&
                            !vkutil::QuerySwapChainSupport(device, instance.get_surface_handle()).presentModes.empty();

    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;

}

void VulkanDevice::find_queue_families(VkPhysicalDevice device)
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
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance.get_surface_handle(), &presentSupport);
        if (presentSupport) 
            indices.presentFamily = i;
            

        if (indices.isComplete())
            break; 


        i++;
    }
    
}

bool VulkanDevice::check_device_extension_support(VkPhysicalDevice device)
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

void VulkanDevice::create_logical_device()
{
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;



    //------------------------------------[TIMELINE_FEATURES]------------------------------------//
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeatures{};
    timelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineFeatures.timelineSemaphore = VK_TRUE;

    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.features = {};
    deviceFeatures2.pNext = &timelineFeatures;
    //-------------------------------------------------------------------------------------------//


    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &deviceFeatures2;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1; 

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

    vkGetDeviceQueue(handle, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(handle, indices.presentFamily.value(), 0, &presentQueue);
    
    std::cout << "Queues handles created\n";

}

vkutil::QueueFamilyIndices VulkanDevice::get_device_indices() const { return indices; }
