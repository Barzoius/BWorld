#pragma once
#include "Instance.hpp"
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class PhysicalDevice
{
public:
    PhysicalDevice(const Instance&);
    ~PhysicalDevice();

    void Initialize();
    
    VkPhysicalDeviceFeatures getFeatures();
    VkPhysicalDevice getHandle() const;

private:
    bool isDeviceSuitable(VkPhysicalDevice);
    void pickDevice();
    int rateDevice(VkPhysicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice);

    
private:
    const Instance& instance;
    VkPhysicalDevice handle{};
    VkPhysicalDeviceFeatures deviceFeatures{};
};