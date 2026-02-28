#pragma once


#ifdef _WIN32
    #include <windows.h> 
    #include <windef.h>
#endif

struct SurfaceInfo {
#ifdef _WIN32
    HWND hwnd = nullptr;
#elif defined(__linux__)
    void* display = nullptr; // might not work 
    void* window  = nullptr;
#endif
};

struct Resolution
{
    int width;
    int height;
};