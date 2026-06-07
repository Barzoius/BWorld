#pragma once
#include <string>
class Bindable
{
public:
    virtual void bind() = 0;
    virtual ~Bindable() = default;

    virtual std::string get_uid() const noexcept
    {
        return "";
    }
};