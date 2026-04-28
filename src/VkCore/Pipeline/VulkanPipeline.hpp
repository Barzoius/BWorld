#pragma once
#include "Shader.hpp"
#include "VkContext.hpp"

#include "RenderPass.hpp"

class VulkanPipeline
{
public:
	VulkanPipeline() = default;
	virtual ~VulkanPipeline() = default;
    void bindPipeline();   
	void Destroy();

};