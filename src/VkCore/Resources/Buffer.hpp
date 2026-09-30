#pragma once

#include "ResourceConfig.hpp"
#include "Allocated.hpp"

#include "VertexSystem.hpp"
#include "UniformSystem.hpp"

#include <memory>

class Buffer;
using BufferPtr = std::unique_ptr<Buffer>;

class VulkanDevice;


///=======================================================================///
///------------------------------BUFFER_CONFIG----------------------------///
///=======================================================================///

class BufferConfig : public RConfig<BufferConfig, VkBufferCreateInfo>
{
private:
    using ParentConfig = RConfig<BufferConfig, VkBufferCreateInfo>; 
public:
    BufferConfig(VkDeviceSize);

    Buffer    build(VulkanDevice&)     const;
    BufferPtr build_ptr(VulkanDevice&) const;

    BufferConfig & with_flags(VkBufferCreateFlags);
    BufferConfig & with_usage(VkBufferUsageFlags);
    BufferConfig & with_alignment(VkDeviceSize);

    VkDeviceSize get_alignment()       const { return m_alignment; }
    
private:
    VkDeviceSize m_alignment{0};
};

///=======================================================================///
///---------------------------------BUFFER--------------------------------///
///=======================================================================///


class Buffer : public alloc::Allocated<VkBuffer>
{
private:
    using ParentBuffer = alloc::Allocated<VkBuffer>;
public:
    Buffer()                           = delete;
    Buffer(const Buffer &)             = delete;
	Buffer(Buffer &&other)             = default;
	Buffer & operator=(const Buffer &) = delete;
	Buffer & operator=(Buffer &&)      = default;

    Buffer(VulkanDevice&, BufferConfig const&);

    ~Buffer();

    VkDeviceAddress get_device_address() const;
    VkDeviceSize    get_size()           const;

    
    void create_vertex_buffer(DVS::VertexBuffer& vbuf);
    void create_index_buffer();
    void create_uniform_buffer();

    VkBuffer get_staging_buffer(VulkanDevice&, VkDeviceSize, const void*);
private:
    VkDeviceSize m_size = 0;
};


///=======================================================================///
///------------------------------BUFFER_CONFIG----------------------------///
///=======================================================================///


inline BufferConfig::BufferConfig(VkDeviceSize size) 
: ParentConfig(VkBufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size}) {}

inline Buffer BufferConfig::build(VulkanDevice& dev) const { return Buffer{dev, *this}; }

inline BufferPtr BufferConfig::build_ptr(VulkanDevice& dev) const
{
    return std::make_unique<Buffer>(dev, *this);
}


inline BufferConfig& BufferConfig::with_flags(VkBufferCreateFlags flags)
{
	this->m_create_info.flags = flags;
	return *this;
}

inline BufferConfig& BufferConfig::with_usage(VkBufferUsageFlags usage)
{
	this->m_create_info.usage = usage;
	return *this;
}

inline BufferConfig& BufferConfig::with_alignment(VkDeviceSize align)
{
	this->m_alignment = align;
	return *this;
}




///=======================================================================///
///---------------------------------BUFFER--------------------------------///
///=======================================================================///


inline Buffer::Buffer(VulkanDevice& dev, BufferConfig const& config)
: ParentBuffer(config.get_allocation_create_info(), nullptr, &dev),
  m_size(config.get_create_info().size) 
  {
    if (!config.get_debug_name().empty())
	{
		this->set_debug_name(config.get_debug_name());
	}

    VkResult result = create(config.get_create_info());

    // std::cout << "VMA create result: " << result << '\n';
    // std::cout << "Name " << this->get_debug_name() << '\n';
    // std::cout << "Size " << this->get_size() << '\n';

    // std::cout << "handle: " << this->get_handle() << '\n';
    // std::cout << "allocation: " << this->get_allocation() << '\n';
    
    

    set_alloc_mem_props();

    //std::cout << "mapped: " << this->get_alloc_info().pMappedData << '\n';
  }



inline Buffer::~Buffer() { this->destroy(); }

inline VkDeviceAddress Buffer::get_device_address() const
{

    VkBufferDeviceAddressInfo addressInfo
    {
        .sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = this->get_handle()
    };

    return vkGetBufferDeviceAddress(get_device().get(), &addressInfo);
}

inline VkDeviceSize Buffer::get_size() const { return m_size; }



inline VkBuffer Buffer::get_staging_buffer(VulkanDevice& dev, VkDeviceSize size, const void* data)
{
    BufferConfig config(size);
    config
    .with_vma_flags(VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
    .with_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    Buffer buffer(dev, config);

    if(data != nullptr) buffer.update(data, size);

    return buffer.get_handle();
}