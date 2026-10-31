#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"


#include <string>

#include <iostream>


namespace bengine_rand
{

    template<typename T>
    T rand(T low, T high);

    template<typename T>
    T rand_vec(float low, float high);

    glm::vec3 rand_rgb_color();
    glm::vec3 rand_normalized_color();

    bool coin_flip();
    uint32_t dice_roll();
    
}

namespace img
{
    void load(const char* path);
}

