#pragma once

#include "ResourceConfig.hpp"
#include "Allocated.hpp"

#include <memory>

class Image;
using ImagePtr = std::unique_ptr<Image>;

class VulkanDevice;

class ImageConfig : public RConfig<ImageConfig, VkImageCreateInfo>
{
private:
    using ParentConfig = RConfig<ImageConfig, VkImageCreateInfo>; 
public:
    ImageConfig(VkDeviceSize);

    Image    build(VulkanDevice&)     const;
    Image    build_ptr(VulkanDevice&) const;

    ImageConfig & with_flags(VkImageCreateFlags flags);
	ImageConfig & with_usage(VkImageUsageFlags usage);
private:
};


class Image : public alloc::Allocated<VkImage>
{
private:

public:
    Image()                           = delete;
    Image(const Image &)             = delete;
	Image(Image &&other)             = default;
	Image & operator=(const Image &) = delete;
	Image & operator=(Image &&)      = default;

    Image(VulkanDevice&, ImageConfig const&);

    uint64_t     get_device_address() const;
    VkDeviceSize get_size()           const;
private:
    VkDeviceSize m_size = 0;
};

