#include "VulkanDevice.hpp"
#include "VkUtils.hpp"
#include <set>

#include "VkLog.hpp"

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

    return indices.is_complete() && extensionsSupported && swapChainAdequate;

}

void VulkanDevice::find_queue_families(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties2> queueFamilies(queueFamilyCount,{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &queueFamilyCount, queueFamilies.data());

    //--------------------------------------[TRANSFER DEDICATED]-----------------------------------//
    for(int i = 0; i < queueFamilies.size(); i++)
    {
        const auto& queueFamily = queueFamilies[i];

        bool compute  = queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT;
        if(!compute) continue;

        bool graphics = queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        
        if(!graphics)
        {
            indices.s_compute = (uint32_t)i;
            break;
        }

        if (!indices.s_compute.has_value())
            indices.s_compute = i; // best available and most probable
    }
    //---------------------------------------------------------------------------------------------//
    //--------------------------------------[COMPUTE DEDICATED]------------------------------------//
    for (int i = 0; i < queueFamilies.size(); i++)
    {
        const auto& queueFamily = queueFamilies[i];

        bool transfer = queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT;
        if (!transfer) continue;

        bool graphics = queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        bool compute  = queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT;

        
        if (!graphics && !compute)
        {
            indices.s_transfer = (uint32_t)i;
            break;
        }

        if (!indices.s_transfer.has_value())
            indices.s_transfer = i; // best available and most probable

    }
    //---------------------------------------------------------------------------------------------//
    //--------------------------------------[GRAPHICS DEDICATED]-----------------------------------//
    for (int i = 0; i < queueFamilies.size(); i++)
    {
        const auto& queueFamily = queueFamilies[i];

        bool graphics = queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance.get_surface_handle(), &presentSupport);

        if(graphics && presentSupport)
        {
            indices.s_graphics = (uint32_t)i;
            break;
        }

    }
    //---------------------------------------------------------------------------------------------//
    //------------------------------------------[FALLBACK]-----------------------------------------//
    if (indices.s_graphics.has_value())
    {
        uint32_t g = indices.s_graphics.value();

        const auto& q = queueFamilies[g].queueFamilyProperties.queueFlags;

        if (!indices.s_compute.has_value())
            if(q & VK_QUEUE_COMPUTE_BIT)
                indices.s_compute = g;
        if (!indices.s_transfer.has_value())
            if (q & VK_QUEUE_TRANSFER_BIT)
                indices.s_transfer = g;
    }
    //---------------------------------------------------------------------------------------------//
    
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
    //------------------------------------[QUEUE_CREATE_INFO]------------------------------------//
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.s_graphics.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    //-------------------------------------------------------------------------------------------//

    //------------------------------------[DYNAMIC_RENDERING]------------------------------------//
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = nullptr,
        .dynamicRendering = VK_TRUE
    };
    //-------------------------------------------------------------------------------------------//
    
    //------------------------------------[TIMELINE_FEATURES]------------------------------------//
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeatures{};
    timelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineFeatures.timelineSemaphore = VK_TRUE;
    timelineFeatures.pNext = &dynamicRenderingFeatures;
    //-------------------------------------------------------------------------------------------//

    //------------------------------------[CORE DEVICE FEATURES]---------------------------------//
    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.features.geometryShader = VK_TRUE;
    deviceFeatures2.features.tessellationShader = VK_TRUE;
    deviceFeatures2.pNext = &timelineFeatures;
    //-------------------------------------------------------------------------------------------//

    VkPhysicalDeviceVulkan14Features supportedFeatures14{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr };
	VkPhysicalDeviceVulkan13Features supportedFeatures13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &supportedFeatures14 };
	VkPhysicalDeviceVulkan12Features supportedFeatures12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &supportedFeatures13 };
	VkPhysicalDeviceFeatures2 supportedFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &supportedFeatures12 };
	vkGetPhysicalDeviceFeatures2(phyD, &supportedFeatures);

    if (!supportedFeatures13.dynamicRendering || !supportedFeatures13.synchronization2 ||
		!supportedFeatures12.timelineSemaphore)
	{
		std::cout<<"Physical device doesn't meet the feature requirements\n";
	
	}

    VkPhysicalDeviceVulkan14Features features14
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
		.pNext = nullptr,
	};
	VkPhysicalDeviceVulkan13Features features13
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.pNext = &features14,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE,
	};
	VkPhysicalDeviceVulkan12Features features12
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.pNext = &features13,
		.timelineSemaphore = VK_TRUE
	};
	VkPhysicalDeviceFeatures2 features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &features12 };



    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &features;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1; 
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
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

    vkGetDeviceQueue(handle, indices.s_graphics.value(), 0, &graphicsQueue);
    
    std::cout << "Queues handles created\n";

}

vkutil::QueueFamilyIndices VulkanDevice::get_device_indices() const { return indices; }

VkPhysicalDevice VulkanDevice::getPhyD() const { return phyD; }

VkDevice VulkanDevice::get() const { return handle; }
