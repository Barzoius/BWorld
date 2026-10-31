#pragma once 

#include "VkResource.hpp"
#include "VkLog.hpp"
#include "VulkanDevice.hpp"

class Sampler : public VkResource<VkSampler>
{
public:
    Sampler(VulkanDevice&, const VkSamplerCreateInfo &);
	Sampler(const Sampler &) = delete;
	Sampler(Sampler &&);

	~Sampler();

	Sampler & operator = (const Sampler &) = delete;
	Sampler & operator = (Sampler &&)      = delete;
};


inline Sampler::Sampler(VulkanDevice& dev, const VkSamplerCreateInfo &info) 
: VkResource{VK_NULL_HANDLE, &dev}
{
	VK_ASSERT_MSG(vkCreateSampler(get_device().get(), &info, nullptr, &get_handle()),
    "Couldn't create sampler");
}

inline Sampler::Sampler(Sampler &&other) 
: VkResource{std::move(other)} {}

inline Sampler::~Sampler()
{
	if (has_device())
		vkDestroySampler(get_device().get(), get_handle(), nullptr);
}