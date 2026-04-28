#include "VulkanPipeline.hpp"
#include "VkContexts/VkSwapChainContext.hpp"
#include "RenderPass.hpp"


#include <memory>

class GraphicsPipeline : public VulkanPipeline
{
public:
    GraphicsPipeline(VkContext&, const VkSwapchainContext&);

    ~GraphicsPipeline() override;

    void createPipeline(const Shader<ShaderType::VERTEX>&, const Shader<ShaderType::FRAGMENT>&, RenderPass&);

    void createPiplineLayout();

    void Destroy();

private:
    void initDynamicStates();
    void initVertexInput();
    void initInputAssembly();
    void initViewportState();
    void initRasterizer();
    void initMultisample();
    void initDepthStencil();
    void initColorBlend();

private:
    std::vector<VkDynamicState> dynamicStatesVec;

    VkPipelineDynamicStateCreateInfo dynamicState{};
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};


    VkViewport viewport{};
    VkRect2D scissor{};
    
private:
    VkContext& context;
    VkSwapchainContext swapchainContext;

    const std::vector<VkDynamicState> dynamicStates = 
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineLayout pipelineLayout{};
    VkPipeline graphicsPipeline;

};