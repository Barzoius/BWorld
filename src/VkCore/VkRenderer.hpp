#include "vulkan/vulkan.h"
#include "Renderer.hpp"

class VkRenderer : public Renderer
{
public:
    void Initialize(void* windowHandle) override;
    void RenderFrame() override;
    void Shutdown() override;

private:
    //VulkanDevice device;
    //VulkanSwapchain swapchain;
    //VulkanPipeline pipeline;
    //VulkanCommands commands;
    //VulkanSync sync;
};