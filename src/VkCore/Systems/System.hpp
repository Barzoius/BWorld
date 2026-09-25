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