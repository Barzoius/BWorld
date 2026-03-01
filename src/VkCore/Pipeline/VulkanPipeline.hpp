#pragma once
#include "Shader.hpp"


class VulkanPipeline
{
public:
    void createPipeline(Shader<ShaderType::VERTEX> vertex, Shader<ShaderType::FRAGMENT> fragment);

};