#include "vulkan/vulkan.h"

class VulkanDevice
{
public:
    void Initialize();
    void Cleanup();
private:
    void createInstance();
private:
    VkDevice device;
    VkInstance instance;
    VkQueue graphicsQueue;
};