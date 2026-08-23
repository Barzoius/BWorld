#pragma once

#include <vector>
#include <type_traits>
#include <assert.h>
#include <string>


namespace DDS
{
    class DescriptorLayout
    {
        enum ElementType
        {
            Uniform,
            Sampler,
            Image,
            Storage,
        };

        DescriptorLayout& append(ElementType type) noexcept
        {
            m_elements.emplace_back(type);
            return *this;
        }

    private:
        std::vector<ElementType> m_elements;
    };
}