#pragma once

#include "Context.hpp"

#include "vulkan/vulkan.h"

#include "Instance.hpp"
#include "VulkanDevice.hpp"

class VkContext : public Context
{
public:
    VkContext();
    ~VkContext() = default;
    VkContext(const VkContext&) = delete;
    VkContext& operator=(const VkContext&) = delete;
    void Initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&) override;
    void Destroy() override;

    const VulkanDevice& get_device() const { return device; }
    const Instance& get_instance() const { return instance; }

    void update_instance_resolution(const Resolution&);
    

private:
    Instance instance;
    VulkanDevice device;
    
};