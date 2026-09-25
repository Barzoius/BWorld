#include "vulkan/vulkan.h"
#include <string>

template <typename Handle>
class VkResource
{
public:	
    VkResource(Handle handle = nullptr, Device<bindingType> *device_ = nullptr);

	VkResource(const VulkanResource &)            = delete;
	VkResource &operator=(const VulkanResource &) = delete;

	VkResource(VulkanResource&&);
	VkResource &operator=(VulkanResource&&);

	virtual ~VulkanResource() = default;

public:
    const std::string  & get_debug_name()  const;

	VulkanDevice       & get_device();
	VulkanDevice const & get_device()      const;

	Handle             & get_handle();
	const Handle       & get_handle()      const;
	uint64_t             get_handle_u64()  const;
    
	VkObjectType         get_object_type() const;
	// VkObjectType const                   &get_resource() const;
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