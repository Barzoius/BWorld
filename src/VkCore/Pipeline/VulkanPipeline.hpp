#pragma once
#include "Shader.hpp"
#include "VkContext.hpp"

#include "RenderPass.hpp"

class VulkanPipeline
{
public:
	VulkanPipeline() = default;
	virtual ~VulkanPipeline() = default;
    virtual void bindPipeline(VkCommandBuffer&) = 0;   
	virtual VkPipeline get_handle() const = 0;
	void Destroy();
};