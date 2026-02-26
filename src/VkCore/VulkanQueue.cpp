#include "VulkanQueue.hpp"

VulkanQueue::VulkanQueue(VkDevice device, uint32_t familyIndex, uint32_t count, float* priorities)
{

}

VkQueue VulkanQueue::getHandle() const
{
    return handle;
}