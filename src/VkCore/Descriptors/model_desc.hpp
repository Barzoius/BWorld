#pragma once

#include <vector>

#include <vulkan/vulkan.h>

struct texture_data
{
    VkSampler s_sampler;
    VkImageView s_imgView;
};

struct model_desc
{
    VkBuffer s_vertexBuf = VK_NULL_HANDLE;
    VkBuffer s_indexBuf = VK_NULL_HANDLE;

    std::vector<VkBuffer> s_uniforms;
    std::vector<texture_data> s_materials;
};
