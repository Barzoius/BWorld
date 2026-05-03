#pragma once

#include "vulkan/vulkan.h"

#include "ShaderType.hpp"

#include "VkContext.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

template<ShaderType Stage>
class Shader
{

public:
    Shader(VkContext&, const std::string&);
    ~Shader();
    VkShaderModule get() const;

    void Destroy();

    VkPipelineShaderStageCreateInfo getStageInfo() const;
private:
    void createShaderModule(const std::vector<char>&);

    std::vector<char> readFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) 
            throw std::runtime_error("failed to open file!");

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

private:
    VkShaderModule handle{};
    VkContext& context;

};



template <ShaderType Stage>
Shader<Stage>::Shader(VkContext& c, const std::string& path) : context(c)
{
    auto code = readFile(path);
    createShaderModule(code);
}

template <ShaderType Stage>
Shader<Stage>::~Shader()
{
    // vkDestroyShaderModule(context.getDevice().get(), handle, nullptr);
}

template <ShaderType Stage>
void Shader<Stage>::Destroy()
{
    vkDestroyShaderModule(context.get_device().get(), handle, nullptr);
}

template <ShaderType Stage>
void Shader<Stage>::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    if (vkCreateShaderModule(context.get_device().get(), &createInfo, nullptr, &handle) != VK_SUCCESS) 
        throw std::runtime_error("failed to create shader module!");

    std::cout<<"Create Shader Module"<<std::endl;
}

template <ShaderType Stage>
VkPipelineShaderStageCreateInfo Shader<Stage>::getStageInfo() const
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = ShaderStageTraits<Stage>::flag;
    info.module = handle;
    info.pName = "main";

    return info;
}

template <ShaderType Stage>
VkShaderModule Shader<Stage>::get() const
{
    return handle;
}

