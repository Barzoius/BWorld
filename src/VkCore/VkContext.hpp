#pragma once



#include "Context.hpp"

#include "vulkan/vulkan.h"

#include "Instance.hpp"
#include "VulkanDevice.hpp"

#include "VMA/vk_mem_alloc.h"

#include "Queue.hpp"

#include "Systems/System.hpp"

class VkContext : public Context
{
public:
    VkContext();
    ~VkContext() = default;
    VkContext(const VkContext&) = delete;
    VkContext& operator=(const VkContext&) = delete;
    void Initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&) override;
    void Destroy() override;

    [[nodiscard]] const VulkanDevice& get_device() const { return device; }
    [[nodiscard]] const Instance& get_instance() const { return instance; }
    [[nodiscard]] const VmaAllocator& get_allocator() const { return vmaAllocator; }

    void update_instance_resolution(const Resolution&);

    System transfer_sys{};
private:
    Instance instance;
    VulkanDevice device;
    VmaAllocator vmaAllocator;


    
};