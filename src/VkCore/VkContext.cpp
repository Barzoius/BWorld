#include "VkContext.hpp"
#include "VkLog.hpp"

#include <cassert>

VkContext::VkContext() : m_instance(), m_device(m_instance), m_descHeapMng() {}


void VkContext::Initialize(const std::vector<const char*>& exts, 
                           const SurfaceInfo& surface, 
                           const Resolution& resolution)
{
    m_instance.initialize(exts, surface, resolution);
    m_device.Initialize();
    m_descAllocator.initialize(m_device.get());
    m_descHeapMng.init(m_device.getPhyD());

    VmaAllocatorCreateInfo info{};
    info.instance       = m_instance.get_handle();
    info.physicalDevice = m_device.getPhyD();
    info.device         = m_device.get();
    info.flags          = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    VK_ASSERT_MSG(vmaCreateAllocator(&info, &m_vmaAllocator), "Failed to create teh VMA allocator!");

    transfer_sys.device = m_device.get();
    transfer_sys.pool   = m_device.get_transfer_pool();
    transfer_sys.queue  = m_device.get_transfer_queue()->s_handle;

    m_descHeapMng.create_buffer_heap(m_device.get(), m_vmaAllocator);
    //delete_buffer(m_descHeapMng.m_descHeapResources, m_vmaAllocator);

};

void VkContext::Destroy()
{
    vkDeviceWaitIdle(m_device.get());
    vmaDestroyAllocator(m_vmaAllocator);

    m_descAllocator.destroy(m_device.get());
    m_device.Destroy(); 
    m_instance.destroy();
}

void VkContext::update_instance_resolution(const Resolution& res)
{
    m_instance.update_resolution(res);
}

