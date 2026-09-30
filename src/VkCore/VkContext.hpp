#pragma once



#include "Context.hpp"

#include "vulkan/vulkan.h"

#include "Instance.hpp"
#include "VulkanDevice.hpp"
#include "VMA/vk_mem_alloc.hpp"
#include "Systems/System.hpp"

class VkContext : public Context
{
public:
    VkContext();
    ~VkContext()                           = default;
    VkContext(const VkContext&)            = delete;
    VkContext& operator=(const VkContext&) = delete;

    void Initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&) override;
    void Destroy() override;

    [[nodiscard]] const VulkanDevice&        get_device()         const { return m_device; }
    [[nodiscard]]       VulkanDevice&        get_device()               { return m_device; }

    [[nodiscard]] const Instance&            get_instance()       const { return m_instance; }
    [[nodiscard]] const VmaAllocator&        get_allocator()      const { return m_vmaAllocator; }

    void update_instance_resolution(const Resolution&);


    System              transfer_sys{};
private:
    Instance            m_instance;
    VulkanDevice        m_device;
    VmaAllocator        m_vmaAllocator;   

};