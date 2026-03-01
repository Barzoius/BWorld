#include "vulkan/vulkan.h"

#include <iostream>

class VulkanImage
{

public:

    void createImage(VkDevice device);
    void createImageView(VkDevice device);
    

private:




private:
    
    VkImage handle{};

    VkImageView imageView{};
    VkFormat imgFormat{};

};