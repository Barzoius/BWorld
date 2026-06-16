#pragma once

#include "vulkan/vulkan.h"


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

    // [[nodiscard]] VkQueue get_gfx_queue() const;
    // [[nodiscard]] VkQueue get_transfer_queue() const;


    [[nodiscard]] const queue_data* get_graphics_queue() const;
    [[nodiscard]] const queue_data* get_transfer_queue() const;
    [[nodiscard]] const queue_data* get_compute_queue() const;


private:

    void pick_device();
    void create_logical_device();
    void create_queue();
    
    

    bool is_device_suitable(VkPhysicalDevice);
    bool check_device_extension_support(VkPhysicalDevice);
    void find_queue_families(VkPhysicalDevice);

    void init_queues();
 
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
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };

    static constexpr float m_queuePriority = 1.0f;
    static constexpr float m_graphicsPriority = 1.0f;
    static constexpr float m_transferPriority = 1.0f;
    static constexpr float m_computePriority = 1.0f;



public:
    std::unique_ptr<VulkanQueue<GraphicsQueueTraits>> m_graphicsQueue;
    std::unique_ptr<VulkanQueue<TransferQueueTraits>> m_transferQueue;

};