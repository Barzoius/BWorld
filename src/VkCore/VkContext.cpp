#include "VkContext.hpp"

#include <cassert>

#include "VMA/vk_mem_alloc.h"

VkContext::VkContext() : instance(), device(instance)
{
    
}
void VkContext::Initialize(const std::vector<const char*>& exts, 
                           const SurfaceInfo& surface, 
                           const Resolution& resolution)
{
    instance.initialize(exts, surface, resolution);
    device.Initialize();

    VmaAllocator allocator;

    VmaAllocatorCreateInfo info{};
    info.instance = instance.get_handle();
    info.physicalDevice = device.getPhyD();
    info.device = device.get();

    VkResult result = vmaCreateAllocator(&info, &allocator);
    assert(result == VK_SUCCESS);

};

void VkContext::Destroy()
{
    vkDeviceWaitIdle(device.get());
    device.Destroy(); 
    instance.destroy();
}

void VkContext::update_instance_resolution(const Resolution& res)
{
    instance.update_resolution(res);
}