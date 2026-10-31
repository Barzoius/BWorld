#include "Image.hpp"


#include "ImageView.hpp"


///=======================================================================///
///----------------------------------IMAGE--------------------------------///
///=======================================================================///
Image::Image(VulkanDevice& dev, VkImage hnd, const VkExtent3D& ext, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sample_count) 
: PImage{hnd, &dev}
{
	m_create_info.extent     = ext;
	// m_create_info.imageType  = find_image_type(extent);
	m_create_info.format     = format;
	m_create_info.samples    = sample_count;
	m_create_info.usage      = usage;

	m_subresource.arrayLayer = m_create_info.arrayLayers = 1;
	m_subresource.mipLevel   = m_create_info.mipLevels   = 1;
}
    
Image::Image(VulkanDevice& dev, ImageConfig const& config)
: PImage{config.get_allocation_create_info(), VK_NULL_HANDLE, &dev},
    m_create_info(config.get_create_info())
{
    if(!config.get_debug_name().empty())
        set_debug_name(config.get_debug_name());

    VkResult result = create(config.get_create_info());

    m_subresource.arrayLayer = m_create_info.arrayLayers;
    m_subresource.mipLevel   = m_create_info.mipLevels;

    set_alloc_mem_props();
}


Image::Image(Image &&other) noexcept
    :
    PImage{std::move(other)},
    m_create_info{std::exchange(other.m_create_info, {})}, 
    m_subresource{std::exchange(other.m_subresource, {})}, 
    m_views(std::exchange(other.m_views, {}))
{
	for(auto &view : m_views)
		view->set_image(*this);
}

Image::~Image()
{
	destroy();
}

inline uint64_t Image::get_device_address() const
{
    return 0u;   
}

inline VkImageType Image::get_type() const
{
    return m_create_info.imageType;
}

inline VkImageUsageFlags Image::get_usage() const
{
    return m_create_info.usage;
}

inline VkFormat Image::get_format() const
{
    return m_create_info.format;
}

inline VkImageTiling Image::get_tiling() const
{
    return m_create_info.tiling;
}

inline VkSampleCountFlagBits Image::get_sample_count() const
{
    return m_create_info.samples;
}

inline uint32_t Image::get_array_layer_count() const
{
    return m_create_info.arrayLayers;
}

inline const VkImageSubresource & Image::get_subresource() const
{
    return m_subresource;
}

inline const VkExtent3D & Image::get_extent() const
{
    return m_create_info.extent;
}

inline std::unordered_set<ImageView*> & Image::get_views()
{
    return m_views;
}


inline VkDeviceSize Image::get_image_required_size() const
{
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(get_device().get(), get_handle(), &mem_requirements);
    return mem_requirements.size;
}

// inline VkImageCompressionPropertiesEXT Image::get_applied_compression() const
// {

// }