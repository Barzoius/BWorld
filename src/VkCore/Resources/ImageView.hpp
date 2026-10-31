#pragma once

#include "VkResource.hpp"
#include "Image.hpp"
#include "VkLog.hpp"

class ImageView : public VkResource<VkImageView>
{
private:
    using Resource = VkResource<VkImageView>;
public:
    ImageView(Image &img, VkImageViewType type, VkFormat format = VK_FORMAT_UNDEFINED,
              uint32_t mip_level = 0,    uint32_t array_layer = 0, 
              uint32_t n_mip_levels = 0, uint32_t n_array_layers = 0);

    ImageView(ImageView &) = delete;
    ImageView(ImageView &&other);
    ~ImageView() override;

    ImageView & operator = (const ImageView &) = delete;
    ImageView & operator = (ImageView &&)      = delete;

    void set_image(Image &image);
    
    VkFormat                 get_format()             const;
    VkImageSubresourceRange  get_subresource_range()  const;
    VkImageSubresourceLayers get_subresource_layers() const;
    const Image &            get_image()              const;
private:
    Image*                  m_image{};
    VkFormat                m_format;
    VkImageSubresourceRange m_subresource_range;
};

inline ImageView::ImageView(Image &img, VkImageViewType type, VkFormat format,
uint32_t mip_level, uint32_t array_layer, uint32_t n_mip_levels, uint32_t n_array_layers)
: Resource{VK_NULL_HANDLE, &img.get_device()}, m_image{&img}, m_format{format}
{
    if(format == VK_FORMAT_UNDEFINED)
        this->m_format = format = m_image->get_format();

    m_subresource_range.baseMipLevel   = mip_level;
    m_subresource_range.baseArrayLayer = array_layer;
    m_subresource_range.levelCount     = n_mip_levels   == 0 ? 
    m_image->get_subresource().mipLevel : n_mip_levels;
    m_subresource_range.layerCount     = n_array_layers == 0 ? 
    m_image->get_subresource().arrayLayer : n_array_layers;

    //m_subresource_range.aspectMask = DEPTH OR COLOR DECISION

    VkImageViewCreateInfo view_info =
    {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image            = m_image->get_handle(),
        .viewType         = type,
        .format           = m_format,
        .subresourceRange = m_subresource_range
    };

    VK_ASSERT_MSG(vkCreateImageView(get_device().get(), &view_info, nullptr, &get_handle()), 
    "Image view could not be created!");


    m_image->get_views().emplace(this);
}


ImageView::ImageView(ImageView &&other) 
: Resource{std::move(other)}, m_image{other.m_image}, m_format{other.m_format}, 
  m_subresource_range{other.m_subresource_range}
{
    auto &views = m_image->get_views();
    views.erase(&other);
    views.emplace(this);
}

ImageView::~ImageView()
{
	if (has_device())
		vkDestroyImageView(get_device().get(), get_handle(), nullptr);
}

const Image &ImageView::get_image() const
{
	assert(m_image && "Image view referrs an invalid image");
	return *m_image;
}

void ImageView::set_image(Image &img)
{
	m_image = &img;
}

VkFormat ImageView::get_format() const
{
	return m_format;
}

VkImageSubresourceRange ImageView::get_subresource_range() const
{
	return m_subresource_range;
}

VkImageSubresourceLayers ImageView::get_subresource_layers() const
{
	VkImageSubresourceLayers subresource{};
	subresource.aspectMask     = m_subresource_range.aspectMask;
	subresource.baseArrayLayer = m_subresource_range.baseArrayLayer;
	subresource.layerCount     = m_subresource_range.layerCount;
	subresource.mipLevel       = m_subresource_range.baseMipLevel;
	return subresource;
}
