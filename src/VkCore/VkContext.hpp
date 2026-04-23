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

    const VulkanDevice& getDevice() const { return device; }
    const Instance& getInstance() const { return instance; }
    

private:
    Instance instance;
    VulkanDevice device;
    
};