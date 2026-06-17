#pragma once

#include "vulkan/vulkan.h"

#include "VertexSystem.hpp"

struct index_buffer_data
{
    VkBuffer s_handle;
};


struct vertex_buffer_data
{
    VkBuffer s_handle;
};

struct uniform_buffer_data
{

};


void create_vertex_buffer(const DVS::VertexBuffer&);
void create_index_buffer();
void create_uniform_buffer();

void create_buffer();