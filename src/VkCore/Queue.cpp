#include "Queue.hpp"
#include <iostream>

bool can_present(const queue_data& data)
{
    return data.s_flags & VK_QUEUE_GRAPHICS_BIT;
}

bool can_transfer(const queue_data& data)
{
    return data.s_flags & VK_QUEUE_TRANSFER_BIT;
}

bool can_compute(const queue_data& data)
{
    return data.s_flags & VK_QUEUE_COMPUTE_BIT;
}

void submit(const queue_data& data)
{

}

void present(const queue_data& data)
{

}

