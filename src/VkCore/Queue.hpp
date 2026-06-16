#pragma once

#include "vulkan/vulkan.h"

struct queue_data
{
    VkQueue s_handle;
    VkQueueFlags s_flags;
    uint32_t s_Familyindex;

    uint32_t s_index;
};

bool can_present(const queue_data&);
bool can_transfer(const queue_data&);
bool can_compute(const queue_data&);

void submit(const queue_data&);
void present(const queue_data&);
void copy_buffer();