#include "VkContext.hpp"

VkContext::VkContext() : instance(), device(instance)
{
    
}
void VkContext::Initialize(const std::vector<const char*>& exts, 
                           const SurfaceInfo& surface, 
                           const Resolution& resolution)
{
    instance.initialize(exts, surface, resolution);
    device.Initialize();
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