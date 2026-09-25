
#include "VkResource.hpp"

#include "VMA/vk_mem_alloc.hpp"

template <typename Handle>
class Allocated : public VkResource<Handle>
{
public:
    using ParentAllocated = VkResource<Handle>
public:
	Allocated()                  = delete;
	Allocated(const Allocated &) = delete;

	Allocated(Allocated &&other) noexcept;

	Allocated &operator=(Allocated const &other) = delete;
	Allocated &operator=(Allocated &&other)      = default;

protected:
    template <typename... Args>
	Allocated(const VmaAllocationCreateInfo &allocation_create_info, Args &&...args);

    Allocated(Handle, VulkanDevice *device_ = nullptr);
public:
    const Handle* get()           const;
    const uint8_t* get_data()     const;
    uint8_t *get_mapped_data()    const;
    VkDeviceMemory get_memory()   const;
    VkDeviceSize get_mem_offset() const;

    uint8_t* map();
    void     unmap();
    bool     mapped() const;

    //these need to change 
    size_t update(const uint8_t *data, size_t size, size_t offset = 0);
    size_t update(void const *data, size_t size, size_t offset = 0);
    template <class T>
	size_t update(const T &object, size_t offset = 0)
	{
		return update(reinterpret_cast<const uint8_t *>(&object), sizeof(T), offset);
	}

    VmaAllocation get_allocation() const;
    void set_allocation(VmaAllocation alloc);
private:
    VmaAllocationCreateInfo m_alloc_create_info = {};
	VmaAllocation           m_alloc             = VK_NULL_HANDLE;

    // pointer to the memory
    // no nullptr only when persitent mapped
  
    uint8_t *               m_mapped_data = nullptr;

    bool m_coherent   = false;
    bool m+persistent = false;
};