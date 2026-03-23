// #include "Shader.hpp"

// #include <fstream>
//#include <iostream>

// template <ShaderType Stage>
// Shader<Stage>::Shader(VkDevice device, const std::string& path)
// {
//     auto code = readFile(path);
//     createShaderModule(device, code);
// }

// template <ShaderType Stage>
// void Shader<Stage>::createShaderModule(VkDevice device, const std::vector<char>& code)
// {
//     VkShaderModuleCreateInfo createInfo{};
//     createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//     createInfo.codeSize = code.size();
//     createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
//     if (vkCreateShaderModule(device, &createInfo, nullptr, &handle) != VK_SUCCESS) 
//         throw std::runtime_error("failed to create shader module!");

//     //std::cout<<"Create Shader Module"<<std::endl;
// }

// template <ShaderType Stage>
// VkPipelineShaderStageCreateInfo Shader<Stage>::getStageInfo() const
// {
//     VkPipelineShaderStageCreateInfo info{};
//     info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     info.stage = ShaderStageTraits<Stage>::flag;
//     info.module = handle;
//     info.pName = "main";

//     return info;
// }

// template <ShaderType Stage>
// VkShaderModule Shader<Stage>::get() const
// {
//     return handle;
// }

// std::vector<char> readFile(const std::string& filePath)
// {
//     std::ifstream file(filePath, std::ios::ate | std::ios::binary);

//     if (!file.is_open()) 
//         throw std::runtime_error("failed to open file!");

//     size_t fileSize = (size_t) file.tellg();
//     std::vector<char> buffer(fileSize);

//     file.seekg(0);
//     file.read(buffer.data(), fileSize);

//     file.close();

//     return buffer;
// }