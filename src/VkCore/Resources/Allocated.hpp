#pragma once

#include "VkResource.hpp"

#include "VMA/vk_mem_alloc.hpp"

#include "VkLog.hpp"

#include "VkContext.hpp"

namespace alloc
{

VmaAllocator &get_memory_allocator();
void initialize_vma_allocator(VkContext& ctx);
void destroy_vma_allocator();


template<typename Handle>
struct VMAOps;

template<>
struct VMAOps<VkBuffer>
{
    using CreateInfo = VkBufferCreateInfo;

    static VkResult create(const VkBufferCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_create_info, VkBuffer* buffer, VmaAllocation* allocation, VmaAllocationInfo* alloc_info)
    {
        return vmaCreateBuffer(get_memory_allocator(), create_info, alloc_create_info, buffer, allocation, alloc_info);  
    }

    
    static void destroy(VkBuffer buffer, VmaAllocation allocation) 
    {
        vmaDestroyBuffer(get_memory_allocator(), buffer, allocation);
    };
};


template<>
struct VMAOps<VkImage>
{
    using  CreateInfo = VkImageCreateInfo;
    static VkResult create(){ return VK_SUCCESS; }
    static void destroy() {};
};


template <typename Handle>
class Allocated : public VkResource<Handle>
{
public:
    using ParentAllocated = VkResource<Handle>;
    using AllocOps = VMAOps<Handle>;
private:
    static constexpr auto VK_MPHC_BIT = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
public:
    Allocated()                  = delete;
    Allocated(const Allocated &) = delete;

    Allocated(Allocated &&other) noexcept;

    Allocated &operator=(Allocated const &other) = delete;
    Allocated &operator=(Allocated &&other)      = default;

    VkResult create(const typename AllocOps::CreateInfo& create_info);
    void     destroy();

protected:
    template <typename... Args>
    Allocated(const VmaAllocationCreateInfo&, Args &&...args);

    Allocated(Handle, VulkanDevice *device = nullptr);
public:
    const Handle* get()           const;
    const uint8_t* get_data()     const;
    uint8_t *get_mapped_data()    const;
    VkDeviceMemory get_memory()   const;
    VkDeviceSize get_mem_offset() const;

    uint8_t* map();
    void     unmap();
    bool     mapped() const;
    void     flush(VkDeviceSize, VkDeviceSize);

    //these need to change 
    size_t update(const uint8_t* data, size_t size, size_t offset = 0);
    size_t update(void const* data, size_t size, size_t offset = 0);
    size_t update(const char* data, size_t size, size_t offset = 0);


    template <class T>
	size_t update(const T &object, size_t offset = 0)
	{
        
		return update(reinterpret_cast<const uint8_t *>(&object), sizeof(T), offset);
	}

    template <typename T>
	size_t update(std::vector<T> const &data, size_t offset = 0)
	{
		return update(data.data(), data.size() * sizeof(T), offset);
	}

    VmaAllocation     get_allocation() const;
    VmaAllocationInfo get_alloc_info() const;
    void set_allocation(VmaAllocation alloc);
    void set_alloc_mem_props();
private:
    VmaAllocationCreateInfo m_alloc_create_info = {};
	VmaAllocation           m_alloc             = VK_NULL_HANDLE;
    VmaAllocationInfo       m_alloc_info        = {};

    // pointer to the memory
    // no nullptr only when persitent mapped
  
    uint8_t * m_mapped_data = nullptr; // pMappedData

    bool m_coherent   = false;
    bool m_persistent = false;
};


template <typename Handle>
inline Allocated<Handle>::Allocated(Allocated &&other) noexcept 
: ParentAllocated{static_cast<ParentAllocated&&>(other)},
  m_alloc_create_info(std::exchange(other.m_alloc_create_info, {})),
  m_alloc(std::exchange(other.m_alloc, {})),
  m_mapped_data(std::exchange(other.m_mapped_data, {})),
  m_coherent(std::exchange(other.m_coherent, {})),
  m_persistent(std::exchange(other.m_persistent, {})) {}



template <typename Handle>
template <typename... Args>
inline Allocated<Handle>::Allocated(const VmaAllocationCreateInfo &alloc_create_info, Args &&...args): ParentAllocated{std::forward<Args>(args)...}, 
  m_alloc_create_info(alloc_create_info) {}

template <typename Handle>
inline Allocated<Handle>::Allocated(Handle handle, VulkanDevice *device)
: ParentAllocated(handle, device) {}


template <typename Handle>
VkResult Allocated<Handle>::create(const typename AllocOps::CreateInfo& create_info)
{ 
    return AllocOps::create(&create_info, &m_alloc_create_info, 
    &this->get_handle(), &m_alloc, &m_alloc_info); 
}

template <typename Handle>
void Allocated<Handle>::destroy() 
{ 
    if(this->has_handle())
    {
        AllocOps::destroy(this->get_handle(), m_alloc);
        this->get_handle() = VK_NULL_HANDLE;
        m_alloc            = VK_NULL_HANDLE;
    }
}

template <typename Handle>
inline const Handle* Allocated<Handle>::get() const
{
    return &ParentAllocated::get_handle();
}
template <typename Handle>
inline const uint8_t* Allocated<Handle>::get_data() const
{
    return m_mapped_data;
}

template <typename Handle>
inline uint8_t* Allocated<Handle>::get_mapped_data() const
{
    return m_mapped_data;
}

template <typename Handle>
inline VkDeviceMemory Allocated<Handle>::get_memory() const
{
    // use m_alloc_info
    VmaAllocationInfo alloc_info;
	vmaGetAllocationInfo(get_memory_allocator(), m_alloc, &alloc_info);
    return alloc_info.deviceMemory;
}

template <typename Handle>
inline VkDeviceSize Allocated<Handle>::get_mem_offset() const
{
    // use m_alloc_info
    VmaAllocationInfo alloc_info;
	vmaGetAllocationInfo(get_memory_allocator(), m_alloc, &alloc_info);
    return alloc_info.offset;
}

template <typename Handle>
inline uint8_t* Allocated<Handle>::map()
{
    if(!m_persistent && !mapped())
    {
        VK_ASSERT_MSG(vmaMapMemory(get_memory_allocator(), m_alloc, reinterpret_cast<void**>(&m_mapped_data)), "VMA::failed to map memeory!");
        assert(m_mapped_data);
    }
    return m_mapped_data;
}

template <typename Handle>
inline void Allocated<Handle>::unmap() 
{ 
    if(!m_persistent && mapped())
    {
        vmaUnmapMemory(get_memory_allocator(), m_alloc);
        m_mapped_data = nullptr;
    }
}

template <typename Handle>
inline bool Allocated<Handle>::mapped() const { return m_mapped_data != nullptr; }

template <typename Handle>

inline void Allocated<Handle>::flush(VkDeviceSize offset, VkDeviceSize size)
{
    if(m_coherent)
        vmaFlushAllocation(get_memory_allocator(), m_alloc, offset, size);
}

template <typename Handle>
inline VmaAllocation Allocated<Handle>::get_allocation() const { return m_alloc; }

template <typename Handle>
inline VmaAllocationInfo Allocated<Handle>::get_alloc_info() const
{
    if(m_alloc != VK_NULL_HANDLE)
        return m_alloc_info;
    // else something :))
    return m_alloc_info;
}
   
template <typename Handle>
inline void Allocated<Handle>::set_allocation(VmaAllocation alloc) { m_alloc = alloc; }

template <typename Handle>
inline void Allocated<Handle>::set_alloc_mem_props()
{
    VkMemoryPropertyFlags mem_props;
    vmaGetAllocationMemoryProperties(get_memory_allocator(), m_alloc, &mem_props);
    m_coherent = (mem_props & VK_MPHC_BIT) == VK_MPHC_BIT;
    m_mapped_data = static_cast<uint8_t *>(m_alloc_info.pMappedData);
    m_persistent = mapped();
}


template <typename Handle>
inline size_t Allocated<Handle>::update(const uint8_t *data, size_t size, size_t offset)
{
    
    if(m_persistent)
    {
        std::copy(data, data + size, m_mapped_data + offset);
        flush(offset, size);
    }
    else
    {
        map();
        std::copy(data, data + size, m_mapped_data + offset);
        flush(offset, size);
        unmap();
    }

    
    return size;
}

template <typename Handle>
inline size_t Allocated<Handle>::update(void const *data, size_t size, size_t offset)
{
	return update(reinterpret_cast<const uint8_t *>(data), size, offset);
}

template <typename Handle>
inline size_t Allocated<Handle>::update(const char* data, size_t size, size_t offset)
{
	return update(reinterpret_cast<const uint8_t *>(data), size, offset);
}

} // alloc namespace end