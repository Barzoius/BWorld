#include "Buffer.hpp"

inline BufferConfig::BufferConfig(VkDeviceSize size) 
: ParentConfig(VkBufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size}) {}

// inline Buffer BufferConfig::build(VulkanDevice& dev) const
// {
//     return ...
// }