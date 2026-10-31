#pragma once

#include "ResourceConfig.hpp"
#include "Allocated.hpp"

#include <memory>
#include <unordered_set>


class Image;
using ImagePtr = std::unique_ptr<Image>;

class VulkanDevice;

///=======================================================================///
///------------------------------IMAGE_CONFIG-----------------------------///
///=======================================================================///

class ImageConfig : public RConfig<ImageConfig, VkImageCreateInfo>
{
private:
    using ParentConfig = RConfig<ImageConfig, VkImageCreateInfo>; 
public:
    ImageConfig(VkExtent3D const &extent);
    ImageConfig(uint32_t width, uint32_t height = 1, uint32_t depth = 1);

    Image    build(VulkanDevice&)     const;
    ImagePtr build_ptr(VulkanDevice&) const;

    ImageConfig & with_flags(VkImageCreateFlags flags);
    ImageConfig & with_usage(VkImageUsageFlags usage);
    ImageConfig & with_image_type(VkImageType type);
    ImageConfig & with_format(VkFormat format);
    ImageConfig & with_mip_levels(uint32_t levels);
    ImageConfig & with_array_layers(uint32_t layers);
    ImageConfig & with_sample_count(VkSampleCountFlagBits count);
    ImageConfig & with_tiling(VkImageTiling tiling);
    ImageConfig & with_extension();

private:
};

///=======================================================================///
///----------------------------------IMAGE--------------------------------///
///=======================================================================///

class ImageView;

class Image : public alloc::Allocated<VkImage>
{
private:
    using PImage = alloc::Allocated<VkImage>;
public:
    Image(VulkanDevice&, VkImage, const VkExtent3D&, VkFormat, VkImageUsageFlags,
	VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);

    Image(VulkanDevice&, ImageConfig const&);
    Image(const Image &) = delete;
    Image(Image &&other) noexcept;
    ~Image() override;

    Image &operator = (const Image &) = delete;
	Image &operator = (Image &&)      = delete;

    uint64_t                         get_device_address()      const;
    VkImageType                      get_type()                const;
    VkImageUsageFlags                get_usage()               const;
    VkFormat                         get_format()              const;
    VkImageTiling                    get_tiling()              const;
    VkSampleCountFlagBits            get_sample_count()        const;
    uint32_t                         get_array_layer_count()   const;
    VkDeviceSize                     get_image_required_size() const;
    VkImageCompressionPropertiesEXT  get_applied_compression() const;

    const VkImageSubresource &       get_subresource()          const;
    const VkExtent3D         &       get_extent()               const;

    std::unordered_set<ImageView*> & get_views();
private:
    VkImageCreateInfo               m_create_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    VkImageSubresource              m_subresource{};
	std::unordered_set<ImageView *> m_views;
};


///=======================================================================///
///------------------------------IMAGE_CONFIG-----------------------------///
///=======================================================================///


inline ImageConfig::ImageConfig(VkExtent3D const &extent) 
: ParentConfig(VkImageCreateInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr}) 
{
    VkImageCreateInfo &create_info = get_create_info();
    create_info.extent             = extent;
    create_info.arrayLayers        = 1;
	create_info.mipLevels          = 1;
	create_info.imageType          = VK_IMAGE_TYPE_2D;
	create_info.format             = VK_FORMAT_R8G8B8A8_UNORM; // make this selectable
	create_info.samples            = VK_SAMPLE_COUNT_1_BIT;
}

inline ImageConfig::ImageConfig(uint32_t width, uint32_t height, uint32_t depth)
: ImageConfig(VkExtent3D{width, height, depth}) {}

inline Image ImageConfig::build(VulkanDevice& dev) const { return Image{dev, *this}; }

inline ImagePtr ImageConfig::build_ptr(VulkanDevice& dev) const
{
    return std::make_unique<Image>(dev, *this);
}

inline ImageConfig & ImageConfig::with_flags(VkBufferCreateFlags flags)
{
	get_create_info().flags = flags;
	return *this;
}

inline ImageConfig & ImageConfig::with_usage(VkBufferUsageFlags usage)
{
	get_create_info().usage = usage;
	return *this;
}

inline ImageConfig & ImageConfig::with_format(VkFormat format)
{
    get_create_info().format = format;
    return *this;
}

inline ImageConfig & ImageConfig::with_image_type(VkImageType type)
{
    get_create_info().imageType = type;
    return *this;
}

inline ImageConfig & ImageConfig::with_mip_levels(uint32_t levels)
{
    get_create_info().mipLevels = levels;
    return *this;
}

inline ImageConfig & ImageConfig::with_array_layers(uint32_t layers)
{
    get_create_info().arrayLayers = layers;
    return *this;
}

inline ImageConfig & ImageConfig::with_sample_count(VkSampleCountFlagBits count)
{
    get_create_info().samples = count;
    return *this;
}

inline ImageConfig & ImageConfig::with_tiling(VkImageTiling tiling)
{
    get_create_info().tiling = tiling;
    return *this;
}

inline ImageConfig & ImageConfig::with_extension()
{
    // get_create_info().pNext = nullptr;
    return *this;
}

