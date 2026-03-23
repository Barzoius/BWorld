#include "VkContext.hpp"

VkContext::VkContext() : instance(), device(instance)
{
    
}
void VkContext::Initialize(const std::vector<const char*>& exts, 
                           const SurfaceInfo& surface, 
                           const Resolution& resolution)
{
    instance.Initialize(exts, surface, resolution);
    device.Initialize();
};

void VkContext::Destroy()
{
    
}