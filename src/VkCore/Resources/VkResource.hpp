#pragma once 

#include "vulkan/vulkan.h"
#include <string>

class VulkanDevice;

template <typename Handle>
class VkResource
{
public:
    VkResource(Handle handle = nullptr, VulkanDevice *device = nullptr);

    VkResource(const VkResource &)            = delete;
    VkResource &operator=(const VkResource &) = delete;

    VkResource(VkResource&&);
    VkResource &operator=(VkResource&&);

    virtual ~VkResource() = default;

public:
    const std::string  & get_debug_name()  const;

    VulkanDevice       & get_device();
    VulkanDevice const & get_device()      const;

    Handle             & get_handle();
    const Handle       & get_handle()      const;
    uint64_t             get_handle_u64()  const;
    
    VkObjectType         get_object_type() const;
public:
    bool has_device() const;
    bool has_handle() const;
public:
    void set_debug_name(const std::string &name);
    void set_handle(Handle hdl);
private:
    std::string    m_debug_name;
    VulkanDevice*  m_device;
    Handle         m_handle;
};

template <typename Handle>
inline VkResource<Handle>::VkResource(Handle handle, VulkanDevice* device) 
: m_handle{handle}, m_device(device) {}


template <typename Handle>
inline VkResource<Handle>::VkResource(VkResource&& other)
: m_handle(std::exchange(other.m_handle, {})),
  m_device(std::exchange(other.m_device, {})),
  m_debug_name(std::exchange(other.m_debug_name, {})) {}


template <typename Handle>
inline VkResource<Handle> &VkResource<Handle>::operator=(VkResource &&other)
{
	m_handle     = std::exchange(other.m_handle, {});
  	m_device     = std::exchange(other.m_device, {});
  	m_debug_name = std::exchange(other.m_debug_name, {});

	return *this;
}

template <typename Handle>
const std::string & VkResource<Handle>::get_debug_name() const { return m_debug_name; }

template <typename Handle>
VulkanDevice & VkResource<Handle>::get_device() { return *m_device; }

template <typename Handle>
VulkanDevice const & VkResource<Handle>::get_device() const { return *m_device; }

template <typename Handle>
Handle & VkResource<Handle>::get_handle() { return m_handle; }

template <typename Handle>
const Handle & VkResource<Handle>::get_handle() const { return m_handle; }

template <typename Handle>
uint64_t VkResource<Handle>::get_handle_u64() const
{
	return 0u;
}
    
template <typename Handle>
VkObjectType VkResource<Handle>::get_object_type() const
{
	// i need a trait map between handles and VkObjectType
}


template <typename Handle>
bool VkResource<Handle>::has_device() const { return m_device != nullptr; }

template <typename Handle>
bool VkResource<Handle>::has_handle() const { return m_handle != nullptr; }


template <typename Handle>
void VkResource<Handle>::set_debug_name(const std::string &name)
{
	m_debug_name = name;
}

template <typename Handle>
void VkResource<Handle>::set_handle(Handle handle)
{
	m_handle = handle;
}