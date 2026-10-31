#include "Utils.hpp"

#include <random>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.hpp"


namespace img
{
    // void load(const char* path)
    // {
    //     int height;
    //     int width;
    //     int channels;

    //     unsigned char* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    //     uint64_t       size   = height * width * 4;

    //     if(!pixels) throw std::runtime_error("failed to load texture image!");
    // }
}


namespace bengine_rand
{
    thread_local std::mt19937 gen(std::random_device{}());

    template<>
    uint32_t rand<uint32_t>(uint32_t low, uint32_t high)
    {
        std::uniform_int_distribution<uint32_t> dist_int(low, high);
        return dist_int(gen);
    }   
    
    template<>
    float rand<float>(float low, float high)
    {
        std::uniform_real_distribution<float> dist_float(low, high);
        return dist_float(gen);
    }

    template<>
    double rand<double>(double low, double high)
    {
        std::uniform_real_distribution<double> dist_double(low, high);
        return dist_double(gen);
    }

    template<>
    glm::vec2 rand_vec<glm::vec2>(float low, float high)
    {
        return glm::vec2(rand<float>(low, high), 
                         rand<float>(low, high));
    }

    template<>
    glm::vec3 rand_vec<glm::vec3>(float low, float high)
    {
        return glm::vec3(rand<float>(low, high), 
                         rand<float>(low, high), 
                         rand<float>(low, high));
    }

    template<>
    glm::vec4 rand_vec<glm::vec4>(float low, float high)
    {
        return glm::vec4(rand<float>(low, high), 
                         rand<float>(low, high),
                         rand<float>(low, high),
                         rand<float>(low, high));
    }

    glm::vec3 rand_rgb_color()
    {
        return glm::vec3(rand<float>(0.0, 255.0), 
                         rand<float>(0.0, 255.0), 
                         rand<float>(0.0, 255.0));
    }
    glm::vec3 rand_normalized_color()
    {
        return glm::vec3(rand<float>(0.0, 1.0), 
                         rand<float>(0.0, 1.0), 
                         rand<float>(0.0, 1.0));
    }

    bool     coin_flip() { return rand<uint32_t>(0, 1) == 1; }
    uint32_t dice_roll() { return rand<uint32_t>(1, 6); }
}