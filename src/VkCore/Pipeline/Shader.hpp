#pragma once

#include "vulkan/vulkan.h"

#include "ShaderType.hpp"

#include <vector>
#include <string>


template<ShaderType Stage>
class Shader
{

public:
    Shader(VkDevice, const std::string&);

    VkShaderModule get() const;

    VkPipelineShaderStageCreateInfo getStageInfo() const;
private:
    static std::vector<char> readFile(const std::string&);
    void createShaderModule(VkDevice, const std::vector<char>&);

private:
    VkShaderModule handle{};

};