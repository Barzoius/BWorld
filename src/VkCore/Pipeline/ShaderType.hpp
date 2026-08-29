#include "vulkan/vulkan.h"

#include "ShaderTypes.hpp"

template<ShaderType>
struct ShaderStageTraits;

template<>
struct ShaderStageTraits<ShaderType::VERTEX>
{
    static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_VERTEX_BIT;
};

template<>
struct ShaderStageTraits<ShaderType::TCS>
{
    static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
};

template<>
struct ShaderStageTraits<ShaderType::TES>
{
    static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
};

template<>
struct ShaderStageTraits<ShaderType::GEOMETRY>
{
    static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_GEOMETRY_BIT;
};

template<>
struct ShaderStageTraits<ShaderType::FRAGMENT>
{
    static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_FRAGMENT_BIT;
};

template<>
struct ShaderStageTraits<ShaderType::COMPUTE>
{
    static constexpr VkShaderStageFlagBits flag = VK_SHADER_STAGE_COMPUTE_BIT;
};