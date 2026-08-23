#pragma once

#include <vector>
#include <type_traits>
#include <assert.h>
#include <string>

#define ELEMENT_TYPES \
    X( Integer ) \
    X( Float ) \
    X( Float2 ) \
    X( Float3 ) \
    X( Float4 ) \
    X( Matrix ) \
    X( Bool )



namespace DUS
{
    enum ElementType
    {
        #define X(elem) elem,
        ELEMENT_TYPES
        #undef X
        Stuct,
        Array,
        Empty,
    };

    template<ElementType> struct Map { static constexpr bool valid = false; };

    template<> struct Map<Integer>
    {
        using SysType = int;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code = "IN";
        static constexpr bool valid = true;
    };
    
    template<> struct Map<Float>
    {
        using SysType = float;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code = "F1";
        static constexpr bool valid = true;
    };

    template<> struct Map<Float2>
    {
        using SysType = int;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code = "F2";
        static constexpr bool valid = true;
    };

    template<> struct Map<Float3>
    {
        using SysType = int;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code = "F3";
        static constexpr bool valid = true;
    };

    template<> struct Map<Float4>
    {
        using SysType = int;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code = "F4";
        static constexpr bool valid = true;
    };

    template<> struct Map<Matrix>
    {
        using SysType = int;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code = "M4";
        static constexpr bool valid = true;
    };

    template<> struct Map<Bool>
    {
        using SysType = bool;
        static constexpr size_t shader_size = 4u;
        static constexpr const char* code = "BL";
        static constexpr bool valid = true;
    };

    #define X(elem) static_assert(Map<elem>::valid, "Type("#elem") has no map implementation!");
    ELEMENT_TYPES
    #undef X

    template<typename T>
    struct ReverseMap { static constexpr bool valid = false; };

    #define X(elem) \
    template<> struct ReverseMap<typename Map<elem>::SysType> \
    { \
        static constexpr ElementType type = elem; \
        static constexpr bool valid = true; \
    };
    ELEMENT_TYPES
    #undef X

    class UniformLayout
    {

    };


    class Uniform
    {

    };


    class Buffer
    {

    };


    class LayoutCodex
    {

    };
}