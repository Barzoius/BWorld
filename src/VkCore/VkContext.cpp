#include "VkContext.hpp"

#include <cassert>

VkContext::VkContext() : instance(), device(instance)
{
    
}
void VkContext::Initialize(const std::vector<const char*>& exts, 
                           const SurfaceInfo& surface, 
                           const Resolution& resolution)
{
    instance.initialize(exts, surface, resolution);
    device.Initialize();


    VmaAllocatorCreateInfo info{};
    info.instance = instance.get_handle();
    info.physicalDevice = device.getPhyD();
    info.device = device.get();

    VkResult result = vmaCreateAllocator(&info, &vmaAllocator);
    assert(result == VK_SUCCESS);

    transfer_sys.device = device.get();
    transfer_sys.pool = device.get_transfer_pool();
    transfer_sys.queue = device.get_transfer_queue()->s_handle;

};

void VkContext::Destroy()
{
    vkDeviceWaitIdle(device.get());
    vmaDestroyAllocator(vmaAllocator);
    device.Destroy(); 
    instance.destroy();
}

void VkContext::update_instance_resolution(const Resolution& res)
{
    instance.update_resolution(res);
}