#pragma once

#include "vulkan/vulkan.h"

#include "VkLog.hpp"

#include "Instance.hpp"
#include "VkUtils.hpp"

#include "VulkanSwapchain.hpp"

#include "VulkanQueue.hpp"

#include "Queue.hpp"

#include <unordered_map>


class VulkanDevice
{
public:
    VulkanDevice(const Instance&);
    ~VulkanDevice();

    void Initialize();
    void Destroy();

    [[nodiscard]] VkDevice get() const;
    [[nodiscard]] VkPhysicalDevice getPhyD() const;
    [[nodiscard]] vkutil::QueueFamilyIndices get_device_indices() const;

    [[nodiscard]] const queue_data* get_graphics_queue() const;
    [[nodiscard]] const queue_data* get_transfer_queue() const;
    [[nodiscard]] const queue_data* get_compute_queue() const;

    [[nodiscard]] VkCommandPool get_transfer_pool() const { return m_transferCommandPool; }

private:

    VkPhysicalDeviceProperties m_properties{};

    struct VulkanDeviceFeatures
    {
        // Supported
        VkPhysicalDeviceFeatures2 supported{};
        VkPhysicalDeviceVulkan12Features supported12{};
        VkPhysicalDeviceVulkan13Features supported13{};
        VkPhysicalDeviceVulkan14Features supported14{};
        VkPhysicalDeviceShaderObjectFeaturesEXT supportedShaderObject{};

        // Enabled
        VkPhysicalDeviceFeatures2 enabled{};
        VkPhysicalDeviceVulkan12Features enabled12{};
        VkPhysicalDeviceVulkan13Features enabled13{};
        VkPhysicalDeviceVulkan14Features enabled14{};
        VkPhysicalDeviceShaderObjectFeaturesEXT enabledShaderObject{};
    }m_features;

private:

    void pick_device();
    void create_logical_device();
    void create_queue();

    void query_features();
    void enable_features();
    bool supports_required_features() const;

    void retrive_device_info();
    VkPhysicalDeviceProperties get_device_info() const;
    void log_device_info() const;

    
    

    bool is_device_suitable(VkPhysicalDevice);
    bool check_device_extension_support(VkPhysicalDevice);
    void find_queue_families(VkPhysicalDevice);

    void init_queues();
 
    void init_tranfer_command_pool();
private:
    const Instance& instance;    
    VkPhysicalDevice phyD{};
    VkDevice handle{};
    VkSurfaceKHR surface{};


    vkutil::QueueFamilyIndices indices;
    std::vector<VkQueueFamilyProperties2> m_queueFamilies;
    std::vector<queue_data> m_queues;
    std::unordered_map<uint32_t, uint32_t> m_familyQueueCount;
    
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        /*VK_EXT_SHADER_OBJECT_EXTENSION_NAME*/ };

    static constexpr float m_queuePriority = 1.0f;
    static constexpr float m_graphicsPriority = 1.0f;
    static constexpr float m_transferPriority = 1.0f;
    static constexpr float m_computePriority = 1.0f;

    VkCommandPool m_transferCommandPool;



};