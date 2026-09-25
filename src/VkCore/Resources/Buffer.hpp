#include "ResourceConfig.hpp"
#include "Allocated.hpp"

#include <memory>

class Buffer;
using BufferPtr = std::unique_ptr<Buffer>;

class VulkanDevice;

class BufferConfig : public RConfig<BufferConfig, VkBufferCreateInfo>
{
private:
    using ParentConfig = RConfig<BufferConfig, VkBufferCreateInfo>; 
public:
    BufferConfig(VkDeviceSize);

    Buffer    build(VulkanDevice&)     const;
    BufferPtr build_ptr(VulkanDevice&) const;

    BufferConfig & with_flags(VkBufferCreateFlags flags);
	BufferConfig & with_usage(VkBufferUsageFlags usage);
	BufferConfig & with_alignment(VkDeviceSize align);

    VkDeviceSize get_alignment()       const { return m_alignment; }
    
private:
    VkDeviceSize m_alignment{0};
};


class Buffer 
{
private:

public:
    Buffer()                           = delete;
    Buffer(const Buffer &)             = delete;
	Buffer(Buffer &&other)             = default;
	Buffer & operator=(const Buffer &) = delete;
	Buffer & operator=(Buffer &&)      = default;

    Buffer(VulkanDevice&, BufferConfig const&);

    uint64_t     get_device_address() const;
    VkDeviceSize get_size()           const;
private:
    VkDeviceSize m_size = 0;
};

