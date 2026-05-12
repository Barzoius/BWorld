#pragma once

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
    #ifdef USE_X11
        #define VK_USE_PLATFORM_XLIB_KHR
    #else // Wayland
        #define VK_USE_PLATFORM_WAYLAND_KHR
    #endif
#endif

#include "vulkan/vulkan.h"
#include <vector>
#include <iostream>

#include "SurfaceInfo.hpp"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class Instance
{
public:
    Instance();

    ~Instance();

    void initialize(const std::vector<const char*>&, const SurfaceInfo&, const Resolution&);
    void destroy();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }


    VkSurfaceKHR get_surface_handle() const;
    Resolution get_resolution() const;
    VkInstance get_handle() const;
    void update_resolution(const Resolution&);

private:
    void create_instance();
    void create_surface();
    void setup_debug_messenger();
    void populate_debug_messenger_createinfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    bool check_extensions();
    bool check_validation_layer_support();
public:
    VkInstance handle{};
    VkDebugUtilsMessengerEXT debugMessenger{};

    std::vector<const char*> requiered_extensions;
    SurfaceInfo surfaceInfo;
    Resolution resolution;
    VkSurfaceKHR surface;
};




static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}