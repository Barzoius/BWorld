#include "VulkanPipeline.hpp"
#include "RenderPass.hpp"
#include "VulkanSwapchain.hpp"

#include <memory>

class GraphicsPipeline : public VulkanPipeline
{
public:
    GraphicsPipeline(VkContext&);

    ~GraphicsPipeline() override;

    void createPipeline(Shader<ShaderType::VERTEX>, Shader<ShaderType::FRAGMENT>, RenderPass&);

    void createPiplineLayout();

private:
    VkPipelineDynamicStateCreateInfo getDynamicStates();
    VkPipelineVertexInputStateCreateInfo getVertexInput();
    VkPipelineInputAssemblyStateCreateInfo getInputAssembly();
    VkPipelineViewportStateCreateInfo getViewportState();
    VkPipelineRasterizationStateCreateInfo getRasterizer();
    VkPipelineMultisampleStateCreateInfo getMultisample();
    VkPipelineDepthStencilStateCreateInfo getDepthStencil();
    VkPipelineColorBlendStateCreateInfo getColorBlend();

private:
    VkContext& context;

    const std::vector<VkDynamicState> dynamicStates = 
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineLayout pipelineLayout{};
    

    VkPipeline graphicsPipeline;

};