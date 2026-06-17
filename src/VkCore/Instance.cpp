#include "Instance.hpp"
#include <unordered_set>
#include <string>

#include "VkLog.hpp"

Instance::Instance() = default;
Instance::~Instance() = default;

void Instance::initialize(const std::vector<const char*>& exts, 
                          const SurfaceInfo& si, 
                          const Resolution& res)
{
    std::cout << "Instance Initialized\n";

    requiered_extensions = exts;
    surfaceInfo = si;
    resolution = res;

    if (enableValidationLayers)
        requiered_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  

    create_instance();
    setup_debug_messenger();
    create_surface();
}

void Instance::destroy()
{
    std::cout << "Instance destroyed\n";

    if (enableValidationLayers) 
        DestroyDebugUtilsMessengerEXT(handle, debugMessenger, nullptr);
    
    vkDestroySurfaceKHR(handle, surface, nullptr);
    vkDestroyInstance(handle, nullptr);
}

void Instance::create_instance()
{
    get_instance_version();

    if (enableValidationLayers && !check_validation_layer_support()) 
        throw std::runtime_error("validation layers requested, but not available!");
    
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Renderer";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "BEngine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, m_instanceVer.major, m_instanceVer.minor, 0);
    appInfo.pNext = nullptr;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = (uint32_t)requiered_extensions.size();
    createInfo.ppEnabledExtensionNames = requiered_extensions.data();
    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        populate_debug_messenger_createinfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(!check_extensions())
        throw std::runtime_error("requiered extensions missing");

    if (vkCreateInstance(&createInfo, nullptr, &handle) != VK_SUCCESS) 
        throw std::runtime_error("failed to create instance!");
    else
        std::cout << "Instance created\n";

}

void Instance::create_surface()
{
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = surfaceInfo.hwnd;
    createInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(handle, &createInfo, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Win32 surface!");
    
    std::cout << "Win32 surface created\n";

#elif defined(__linux__)
#ifdef USE_X11
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = static_cast<Display*>(info.display);
    createInfo.window = static_cast<Window>(reinterpret_cast<uintptr_t>(info.window));

    if (vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create X11 surface!");
    
    std::cout << "X11 surface created\n";
    
#else // Wayland
    VkWaylandSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.display = static_cast<wl_display*>(info.display);
    createInfo.surface = static_cast<wl_surface*>(info.window);

    if (vkCreateWaylandSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Wayland surface!");
    
    std::cout << "WAYLAND surface created\n";
    
#endif
#endif
}

VkSurfaceKHR Instance::get_surface_handle() const { return surface; }

Resolution Instance::get_resolution() const { return resolution; }

void Instance::update_resolution(const Resolution& res){ resolution = res; }

bool Instance::check_extensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::unordered_set<std::string> available_extensions;
    for (const auto& ext : extensions) 
        available_extensions.insert(ext.extensionName);
    
    for(const char* requierd : requiered_extensions)
    {
        if(available_extensions.find(requierd) == available_extensions.end())
            return false;
    }

    return true;

}


bool Instance::check_validation_layer_support()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}


void Instance::setup_debug_messenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populate_debug_messenger_createinfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(handle, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
        throw std::runtime_error("failed to set up debug messenger!");

    
}

void Instance::populate_debug_messenger_createinfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
}

void Instance::get_instance_version()
{
    uint32_t instanceVer = 0;

    VkResult res = vkEnumerateInstanceVersion(&instanceVer);

    VK_CHECK(res);

    if(res != VK_SUCCESS)
        throw std::runtime_error("no instance versions???");

    m_instanceVer.major = VK_API_VERSION_MAJOR(instanceVer);
    m_instanceVer.minor = VK_API_VERSION_MINOR(instanceVer);
    m_instanceVer.patch = VK_API_VERSION_PATCH(instanceVer);

}

VkInstance Instance::get_handle() const
{
    return handle;
}