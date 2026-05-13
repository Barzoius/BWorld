#pragma once

#include "vulkan/vulkan.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <ctime>
#include <cassert>
#include <cstdarg>
#include <vector>



class VkLogger
{
public:
    enum class LEVEL
    {
        INFO,
        WARNING,
        VK_ERROR,
        CRITICAL
    };

    static VkLogger& Instance()
    {
        static VkLogger instance;
        return instance;
    }

    void SetLogFile(const std::string& filename)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file.open(filename, std::ios::out | std::ios::app);
    }

    void Log(LEVEL lvl, const std::string& msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string out = format(lvl, msg);

        std::cout << out << std::endl;

        if (m_file.is_open()) {
            m_file << out << std::endl;
        }
    }

private:
    VkLogger() = default;

    std::string level_to_string(LEVEL lvl)
    {
        switch(lvl)
        {
            case LEVEL::INFO: return "INFO";
            case LEVEL::WARNING: return "WARNING";
            case LEVEL::VK_ERROR: return "ERROR";
            case LEVEL::CRITICAL: return "CRITICAL";
        }
        return "UNKNOWN";
    }

    std::string timestamp()
    {
        std::time_t now = std::time(nullptr);

        std::tm tm{};

    #ifdef _WIN32
        localtime_s(&tm, &now);
    #else
        localtime_r(&now, &tm);
    #endif

        char buf[64];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
        return buf;
    }

    std::string format(LEVEL lvl, const std::string& msg)
    {
        return "[" + timestamp() + "] [" + level_to_string(lvl) + "] " + msg;
    }

private:
    std::mutex m_mutex;
    std::ofstream m_file;
};



inline std::string format_string(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    return std::string(buffer);
}


#define LLOGI(fmt, ...) VkLogger::Instance().Log(VkLogger::LEVEL::INFO, format_string(fmt,##__VA_ARGS__))
#define LLOGW(fmt, ...) VkLogger::Instance().Log(VkLogger::LEVEL::WARNING, format_string(fmt, ##__VA_ARGS__))
#define LLOGE(fmt, ...) VkLogger::Instance().Log(VkLogger::LEVEL::VK_ERROR, format_string(fmt, ##__VA_ARGS__))
#define LLOGC(fmt, ...) VkLogger::Instance().Log(VkLogger::LEVEL::CRITICAL, format_string(fmt, ##__VA_ARGS__))


inline const char* ivkGetVulkanResultString(VkResult result) {
    switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        default: return "UNKNOWN_VK_RESULT";
    }
}

#define VK_ASSERT(func) do { \
    VkResult vk_assert_result = (func); \
    if (vk_assert_result != VK_SUCCESS) { \
        LLOGW("Vulkan API call failed: %s:%d\n  %s\n  %s", \
            __FILE__, __LINE__, \
            #func, \
            ivkGetVulkanResultString(vk_assert_result)); \
        assert(false); \
    } \
} while (0)

#define VK_ASSERT_RETURN(func, failValue) do { \
    VkResult vk_assert_result = (func); \
    if (vk_assert_result != VK_SUCCESS) { \
        LLOGW("Vulkan API call failed: %s:%d\n  %s\n  %s", \
            __FILE__, __LINE__, \
            #func, \
            ivkGetVulkanResultString(vk_assert_result)); \
        assert(false); \
        return (failValue); \
    } \
} while (0)

#define VK_CHECK(func) do { \
    VkResult vk_check_result = (func); \
    if (vk_check_result != VK_SUCCESS) { \
        LLOGW("Vulkan warning: %s:%d\n  %s\n  %s", \
            __FILE__, __LINE__, \
            #func, \
            ivkGetVulkanResultString(vk_check_result)); \
    } \
} while (0)