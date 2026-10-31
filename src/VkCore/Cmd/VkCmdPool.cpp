#include "VkCmdPool.hpp"
#include "VulkanDevice.hpp"



VkCmdPool::VkCmdPool(VulkanDevice& dev, uint32_t index) 
: m_device{dev}, m_queue_family_index{index}
{
    //uint32_t index = m_device.get_device_indices().s_transfer.value();
    VkCommandPoolCreateInfo info
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = index
    };
    m_handle = m_device.create_cmd_pool(info);
}

VkCmdPool::VkCmdPool(VkCmdPool && other)
: m_device{other.m_device},
  m_handle{other.m_handle},
  m_queue_family_index{other.m_queue_family_index}
{
    other.m_handle = VK_NULL_HANDLE;
}

VkCmdPool::~VkCmdPool()
{
    if(m_handle != VK_NULL_HANDLE)
        m_device.destroy_cmd_pool(m_handle);
}

void VkCmdPool::reset()
{
    vkResetCommandPool(m_device.get(), m_handle, 0);
}


VulkanDevice & VkCmdPool::get_device() { return m_device; }

VkCommandPool VkCmdPool::get_handle() const { return m_handle; }

uint32_t VkCmdPool::get_queue_family_index() const { return m_queue_family_index; }


// std::shared_ptr<VkCmdBuffer> VkCmdPool::request_command_buffer()
// {

// }