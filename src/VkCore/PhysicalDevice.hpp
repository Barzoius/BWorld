#pragma once
#include "Instance.hpp"

class PhysicalDevice
{
public:
    PhysicalDevice(const Instance&);
    ~PhysicalDevice();

private:
    bool isDeviceSuitable(VkPhysicalDevice device);
    void pickDevice();
    int rateDevice(VkPhysicalDevice device);
    
private:
    const Instance& instance;
    VkPhysicalDevice handle{};
};