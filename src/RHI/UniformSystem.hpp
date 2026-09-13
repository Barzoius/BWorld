#pragma once

#include <vector>
#include <type_traits>
#include <string>
#include <optional>
#include <memory>

#include "cassert"



#define ELEMENT_TYPES \
    X( Integer ) \
    X( Float ) \
    X( Float2 ) \
    X( Float3 ) \
    X( Float4 ) \
    X( Mat4x4 ) \
    X( Bool )



namespace DUS
{
    struct FLOAT2
    {
        float x;
        float y;
    };

    struct FLOAT3
    {
        float x;
        float y;
        float z;
    };

    struct FLOAT4
    {
        float x;
        float y;
        float z;
        float w;
    };

    struct MAT2x2 { float data[ 4]; };
    struct MAT3x3 { float data[ 9]; };
    struct MAT4x4 { float data[16]; };

    enum ElementType
    {
        #define X(elem) elem,
        ELEMENT_TYPES
        #undef X
        Struct,
        Array,
        Empty,
    };

    template<ElementType> struct Map { static constexpr bool valid = false; };

    template<> struct Map<Integer>
    {
        using SysType = int;

        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code   = "IN";
        static constexpr bool valid         = true;
    };
    
    template<> struct Map<Float>
    {
        using SysType = float;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code   = "F1";
        static constexpr bool valid         = true;
    };

    template<> struct Map<Float2>
    {
        using SysType = FLOAT2;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code   = "F2";
        static constexpr bool valid         = true;
    };

    template<> struct Map<Float3>
    {
        using SysType = FLOAT3;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code   = "F3";
        static constexpr bool valid         = true;
    };

    template<> struct Map<Float4>
    {
        using SysType = FLOAT4;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code   = "F4";
        static constexpr bool valid         = true;
    };

    template<> struct Map<Mat4x4>
    {
        using SysType = MAT4x4;
        static constexpr size_t shader_size = sizeof(SysType);
        static constexpr const char* code   = "M4";
        static constexpr bool valid         = true;
    };

    template<> struct Map<Bool>
    {
        using SysType = bool;
        static constexpr size_t shader_size = 4u;
        static constexpr const char* code   = "BL";
        static constexpr bool valid         = true;
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


    struct STD140_Policy
    {
        static constexpr size_t alignment(ElementType type) noexcept
        {
            switch( type )
            {

            };

            return 0;
        }

        static constexpr size_t size(ElementType type) noexcept
        {
            switch( type )
            {

            };

            return 0;
        }
    };

    struct STD430_Policy
    {
        static constexpr size_t alignment(ElementType type) noexcept
        {
            switch( type )
            {

            };

            return 0;
        }

        static constexpr size_t size(ElementType type) noexcept
        {
            switch( type )
            {

            };

            return 0;
        }
    };

    struct HLSL_Policy
    {

    };

    class Element
    {
    private:
        struct ExtraDataBase { virtual ~ExtraDataBase() = default; };

        friend class  RawLayout;
        friend struct ExtraData;

    public:
        std::string get_sig() const noexcept;

        bool exists() const noexcept;

        std::pair<size_t, const Element*> calc_index_offset(size_t offset, size_t index) const noexcept;

        Element&       operator[](const std::string& key)       noexcept;
        const Element& operator[](const std::string& key) const noexcept;

        Element&       T()       noexcept;
        const Element& T() const noexcept;


        size_t get_offset_begin()  const noexcept;
        size_t get_offset_end()    const noexcept;
        size_t get_size_in_bytes() const noexcept;

        Element& add(ElementType type, std::string name) noexcept;
        template<ElementType type>
        Element& add(std::string key) noexcept { return add(type, std::move(key)); }

        Element& set(ElementType type, size_t size)      noexcept;
        template<ElementType type>
        Element& set(size_t size)     noexcept { return set(type, size); }


        template<typename T>
        size_t resolve() const noexcept
        {
            switch( type )
            {
            #define X(elem) 
            case elem: assert(typeid(Mape<elem>::SysType) == typeid(T)); return *m_offset;
            ELEMENT_TYPES
            #undef X
            default:
                assert("Element type non-existent." && false);
                return 0u;
            }
        }

    private:
        Element()                 noexcept = default;
        Element(ElementType type) noexcept;

        size_t finalize(size_t offset) noexcept;


        std::string get_sig_for_struct() const noexcept;
        std::string get_sig_for_array()  const noexcept;

        size_t finalize_for_struct(size_t offset);
        size_t finalize_for_array(size_t offset);


        static Element& get_empty_element() noexcept
        {
            static Element empty{}; return empty;
        }


        static size_t advance_to_boundry(size_t offset)                      noexcept;
        static size_t advance_if_crosses_boundry(size_t offset, size_t size) noexcept;
        static bool   crosses_boundry(size_t offset, size_t size)            noexcept;
        static bool   validate_symbol_name(const std::string& name)          noexcept;



    private:
        std::optional<size_t>          m_offset;
        ElementType                    m_type        = Empty;
        std::unique_ptr<ExtraDataBase> m_pExtraData;
    };

    class Layout
    {
        friend class Buffer;
    public:
        size_t      get_size_in_bytes() const noexcept;
        std::string get_sig()           const noexcept;
    protected:
        Layout(std::shared_ptr<Element> root) noexcept;
    protected:
        std::shared_ptr<Element> m_root;
    };


    class RawLayout : public Layout
    {
    public:
        RawLayout() noexcept;

        Element& operator[](const std::string& key) noexcept;

        template<ElementType type>
        Element& add(const std::string& key) noexcept { return m_root->add<tyoe>(key); }
    private:
        void                     clear_root()   noexcept;
        std::shared_ptr<Element> deliver_root() noexcept;
    };

    class CookedLayout : public Layout
    {
        friend class Buffer;
    public:
        const Element& operator[](const std::string& key) const noexcept;

        std::shared_ptr<Element> share_root() const noexcept;
    private:
        CookedLayout(std::shared_ptr<Element> root) noexcept;
        std::shared_ptr<Element> relinquish_root() const noexcept;
    };

    class ConstElementRef
    {
        friend class Buffer;
        friend class ElementRef;
    public:
        class Ptr
        {
            friend class ConstElementRef;
        public:
            template<typename T>
            operator const T*() const noexcept
            {
                static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used for pointer conversion");
                return &static_cast<const T&>(*ref);
            }
        private:
            Ptr(const ConstElementRef* ref) noexcept;
        private:
            const ConstElementRef* ref;
        };
    public:
        bool exists() const noexcept;

        ConstElementRef operator[](const std::string& key) const noexcept;
        ConstElementRef operator[](size_t index)           const noexcept;

        Ptr             operator&()                        const noexcept;

        template<typename T>
        operator const T&() const noexcept
        {
            static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used for conversion");
            return *reinterpret_cast<const T*>(m_bytes + m_offset + m_layout->resolve<T>())
        }
    private:
        ConstElementRef(const Element*, const char*, size_t) noexcept;
    private:
        size_t         m_offset;
        const Element* m_layout;
        const char*    m_bytes;
    };

    class ElementRef
    {
        friend class Buffer;
    public:
        class Ptr
        {
            friend class ElementRef;
        public:
            template<typename T>
            operator const T*() const noexcept
            {
                static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used for pointer conversion");
                return &static_cast<const T&>(*ref);
            }
        private:
            Ptr(const ElementRef*) noexcept;
        private:
            const ElementRef* m_ref;
        };
    public:
        operator ConstElementRef() const noexcept;

        bool exists() const noexcept;

		ElementRef operator[](const std::string& key) const noexcept;
		ElementRef operator[](size_t index)           const noexcept;

        template<typename T>
        bool set_if_exits(const T& val) noexcept
        {
            if(exits()) { *this = val; return true; }
            return false;
        }

        Ptr operator&() const noexcept;

        template<typename T>
        operator T&() const noexcept
        {
            static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in conversion");
            return *reinterpret_cast<T*>(pBytes + offset + pLayout->Resolve<T>());
        }

        template<typename T>
        T& operator=( const T& rhs ) const noexcept
        {
            static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in assignment");
            return static_cast<T&>(*this) = rhs;
        }
    private:
        ElementRef(const Element*, char*, size_t) noexcept;
    private:
        size_t         m_offset;
        const Element* m_layout;
        char*          m_bytes;
    };

    class Buffer
    {
    public:
        Buffer(RawLayout&&)         noexcept;
        Buffer(const CookedLayout&) noexcept;
        Buffer(CookedLayout&&)      noexcept;
        Buffer(const Buffer& )      noexcept;

        Buffer(Buffer&&)            noexcept;

        ElementRef      operator[](const std::string& key)       noexcept;
        ConstElementRef operator[](const std::string& key) const noexcept;

        const char*     get_data()         const noexcept;
        size_t         get_size_in_bytes() const noexcept;
        const Element& get_root_element()  const noexcept;

        void copy_from(const Buffer&) noexcept;

        std::shared_ptr<Element> share_root() const noexcept;

    private:
        std::shared_ptr<Element> m_pLayoutRoot;
        std::vector<char>        m_bytes;
    };


    class LayoutCodex
    {

    };
}

#ifndef DUS_IMPL_SOURCE
#undef ELEMENT_TYPES
#endif