#pragma once

#include <string>

class IResource
{
public:
    virtual ~IResource() = default;

    virtual std::string get_uid() const noexcept;
    
};