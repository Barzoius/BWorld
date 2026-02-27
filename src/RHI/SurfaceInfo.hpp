#pragma once

struct SurfaceInfo {
#ifdef _WIN32
    void* hwnd = nullptr;
#elif defined(__linux__)
    void* display = nullptr;
    void* window  = nullptr;
#endif
};