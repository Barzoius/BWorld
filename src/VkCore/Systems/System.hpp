#pragma once

#include "vulkan/vulkan.h"

#include <iostream>

// i think i will fully remove this "System" concept
struct System
{
    VkDevice device;
    VkQueue queue;
    VkCommandPool pool;
};

void copy_buffer(const System& sys, VkBuffer src, VkBuffer dst, VkDeviceSize size);

void trasition_img_layout(const System& sys, VkImage image, VkFormat format, VkImageLayout oldL, VkImageLayout newL);

void copy_buffer_to_img(const System& sys, VkBuffer src, VkImage dst, uint32_t width, uint32_t height);
