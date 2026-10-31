#pragma once

#include "vulkan/vulkan.h"
#include <memory>
#include <vector>

// #include "VkUtils.hpp"

class VkCmdBuffer;
class VulkanDevice;

class VkCmdPool
{
public:
    VkCmdPool(VulkanDevice&, uint32_t);
    VkCmdPool(VkCmdPool const &) = delete;
    VkCmdPool(VkCmdPool &&);

    VkCmdPool & operator = (VkCmdPool const &) = delete;
    VkCmdPool & operator = (VkCmdPool &&)      = delete;

    ~VkCmdPool();
public:
    VulkanDevice & get_device();
    VkCommandPool  get_handle() const;
    uint32_t       get_queue_family_index() const;

    void           reset();

    std::shared_ptr<VkCmdBuffer> request_command_buffer();

private:
    VulkanDevice & m_device;
    VkCommandPool  m_handle             = VK_NULL_HANDLE;
    uint32_t       m_queue_family_index = 0;
    
    std::vector<std::shared_ptr<VkCmdBuffer>> m_primary_buffers;
    std::vector<std::shared_ptr<VkCmdBuffer>> m_secondary_buffers;

};