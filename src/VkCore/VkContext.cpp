#include "VkContext.hpp"
#include "VkLog.hpp"

#include <cassert>

VkContext::VkContext() : m_instance(), m_device(m_instance) {}


void VkContext::Initialize(const std::vector<const char*>& exts, 
                           const SurfaceInfo& surface, 
                           const Resolution& resolution)
{
    m_instance.initialize(exts, surface, resolution);
    m_device.Initialize();

    VmaAllocatorCreateInfo info{};
    info.instance       = m_instance.get_handle();
    info.physicalDevice = m_device.getPhyD();
    info.device         = m_device.get();
    info.flags          = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    VK_ASSERT_MSG(vmaCreateAllocator(&info, &m_vmaAllocator), "Failed to create the VMA allocator!");

    transfer_sys.device = m_device.get();
    transfer_sys.pool   = m_device.get_transfer_pool();
    transfer_sys.queue  = m_device.get_transfer_queue()->s_handle;

    //delete_buffer(m_descHeapMng.m_descHeapResources, m_vmaAllocator);

};

void VkContext::Destroy()
{
    vkDeviceWaitIdle(m_device.get());
    vmaDestroyAllocator(m_vmaAllocator);
    m_device.Destroy(); 
    m_instance.destroy();
}

void VkContext::update_instance_resolution(const Resolution& res)
{
    m_instance.update_resolution(res);
}

