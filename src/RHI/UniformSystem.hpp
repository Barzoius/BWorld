// #pragma once

// #include <vector>
// #include <type_traits>
// #include <string>
// #include <optional>
// #include <memory>
// #include <unordered_map>

// #include "cassert"



// #define ELEMENT_TYPES \
//     X( Integer ) \
//     X( Float ) \
//     X( Float2 ) \
//     X( Float3 ) \
//     X( Float4 ) \
//     X( Mat4x4 ) \
//     X( Bool )



// namespace DUS
// {
//     struct FLOAT2
//     {
//         float x;
//         float y;
//     };

//     struct FLOAT3
//     {
//         float x;
//         float y;
//         float z;
//     };

//     struct FLOAT4
//     {
//         float x;
//         float y;
//         float z;
//         float w;
//     };

//     struct MAT2x2 { float data[ 4]; };
//     struct MAT3x3 { float data[ 9]; };
//     struct MAT4x4 { float data[16]; };

//     enum ElementType
//     {
//         #define X(elem) elem,
//         ELEMENT_TYPES
//         #undef X
//         Struct,
//         Array,
//         Empty,
//     };

//     template<ElementType> struct Map { static constexpr bool valid = false; };

//     template<> struct Map<Integer>
//     {
//         using SysType = int;

//         static constexpr size_t shader_size = sizeof(SysType);
//         static constexpr const char* code   = "IN";
//         static constexpr bool valid         = true;
//     };
    
//     template<> struct Map<Float>
//     {
//         using SysType = float;
//         static constexpr size_t shader_size = sizeof(SysType);
//         static constexpr const char* code   = "F1";
//         static constexpr bool valid         = true;
//     };

//     template<> struct Map<Float2>
//     {
//         using SysType = FLOAT2;
//         static constexpr size_t shader_size = sizeof(SysType);
//         static constexpr const char* code   = "F2";
//         static constexpr bool valid         = true;
//     };

//     template<> struct Map<Float3>
//     {
//         using SysType = FLOAT3;
//         static constexpr size_t shader_size = sizeof(SysType);
//         static constexpr const char* code   = "F3";
//         static constexpr bool valid         = true;
//     };

//     template<> struct Map<Float4>
//     {
//         using SysType = FLOAT4;
//         static constexpr size_t shader_size = sizeof(SysType);
//         static constexpr const char* code   = "F4";
//         static constexpr bool valid         = true;
//     };

//     template<> struct Map<Mat4x4>
//     {
//         using SysType = MAT4x4;
//         static constexpr size_t shader_size = sizeof(SysType);
//         static constexpr const char* code   = "M4";
//         static constexpr bool valid         = true;
//     };

//     template<> struct Map<Bool>
//     {
//         using SysType = bool;
//         static constexpr size_t shader_size = 4u;
//         static constexpr const char* code   = "BL";
//         static constexpr bool valid         = true;
//     };

//     #define X(elem) static_assert(Map<elem>::valid, "Type("#elem") has no map implementation!");
//     ELEMENT_TYPES
//     #undef X

//     template<typename T>
//     struct ReverseMap { static constexpr bool valid = false; };

//     #define X(elem) \
//     template<> struct ReverseMap<typename Map<elem>::SysType> \
//     { \
//         static constexpr ElementType type = elem; \
//         static constexpr bool valid = true; \
//     };
//     ELEMENT_TYPES
//     #undef X


//     struct STD140_Policy
//     {
//         static size_t place(size_t offset, size_t size) noexcept
//         {
//             return 0u;
//         }
//         static constexpr size_t alignment(ElementType type) noexcept
//         {
//             switch( type )
//             {
//                 case Float:  return 4u;
//                 case Float2: return 8u;
//                 case Float3: return 16u;
//                 case Float4: return 16u;
//                 case Mat4x4: return 16u;

//                 default: assert(false); return 0u;

//             };

//             return 0u;
//         }

//         static size_t advance_to_boundary(size_t offset) noexcept
//         {
//             return 1u;
//         }

//     };

//     struct STD430_Policy
//     {

//         static size_t place(size_t offset, size_t size) noexcept
//         {
//             return 1u;
//         }
//         static constexpr size_t alignment(ElementType type) noexcept
//         {
//             switch( type )
//             {
//                 case Float:  return 4u;
//                 case Float2: return 8u;
//                 case Float3: return 16u;
//                 case Float4: return 16u;
//                 case Mat4x4: return 16u;
                
//                 //case Struct: struct_alignment(type);
//                 //case Array:  array_alignment(type);

//                 default: assert(false); return 0u;

//             };

//             return 0u;
//         }

//         static size_t advance_to_boundary(size_t offset) noexcept
//         {
//             return 1u;
//         }
//     };

//     struct HLSL_Policy
//     {
//         static size_t place(size_t offset, size_t size) noexcept
//         {
            
//             return advance_if_crosses_boundary(offset, size);
//         }

//         // this one is useless for hlsl
//         static size_t align(size_t offset, size_t alignment) noexcept { return offset; }

//         static bool crosses_boundary(size_t offset, size_t size) noexcept
//         {
//             const auto end       = offset + size;
//             const auto pageStart = offset / 16u;
//             const auto pageEnd   = end / 16u;

//             return (pageStart != pageEnd && end % 16u != 0u) || size > 16u;
//         }

        
//         static size_t advance_if_crosses_boundary(size_t offset, size_t size) noexcept
//         {
//             return crosses_boundary(offset, size) ? advance_to_boundary(offset) : offset;
//         }

//         static size_t advance_to_boundary(size_t offset) noexcept
//         {
//             return offset + (16u - offset % 16u) % 16u;
//         }
//     };

//     class Element
//     {
//     private:
//         struct ExtraDataBase { virtual ~ExtraDataBase() = default; };
        
//         size_t   get_struct_element_count() const;
//         Element& get_struct_element(size_t index);

//         friend class  RawLayout;
//         friend struct ExtraData;

//     public:
//         std::string get_sig() const noexcept;

//         bool exists() const noexcept;

//         std::pair<size_t, const Element*> calc_index_offset(size_t offset, size_t index) const noexcept;

//         Element&       operator[](const std::string& key)       noexcept;
//         const Element& operator[](const std::string& key) const noexcept;

//         Element&       T()       noexcept;
//         const Element& T() const noexcept;


//         size_t get_offset_begin()  const noexcept;
        
//         size_t get_offset_end()    const noexcept;
//         size_t get_size_in_bytes() const noexcept;

//         Element& add(ElementType type, std::string name) noexcept;
//         template<ElementType type>
//         Element& add(std::string key) noexcept { return add(type, std::move(key)); }

//         Element& set(ElementType type, size_t size)      noexcept;
//         template<ElementType type>
//         Element& set(size_t size)     noexcept { return set(type, size); }


//         template<typename T>
//         size_t resolve() const noexcept
//         {
//             switch( m_type )
//             {
//             #define X(elem) case elem: assert(typeid(Map<elem>::SysType) == typeid(T)); return *m_offset;
//             ELEMENT_TYPES
//             #undef X
//             default:
//                 assert("Element type non-existent." && false);
//                 return 0u;
//             }
//         }

//     private:
//         Element()                 noexcept = default;
//         Element(ElementType type) noexcept;

//         std::string get_sig_for_struct() const noexcept;
//         std::string get_sig_for_array()  const noexcept;

//         template<class Policy>
//         size_t finalize(size_t offset) noexcept
//         {
//             switch( m_type )
//             {
//             #define X(elem) case elem: m_offset = Policy::place(offset ,Map<elem>::shader_size); return *m_offset + Map<elem>::shader_size;
//             ELEMENT_TYPES
//             #undef X
//             case Struct: return finalize_for_struct<Policy>(offset);
//             case Array : return finalize_for_array<Policy>(offset);
//             default    : assert("Attempt finalization on bad type" && false); return 0u;
//             }
//         }

//         template<class Policy>
//         size_t finalize_for_struct(size_t offset)
//         {
//             const auto count = get_struct_element_count();
//             assert(count != 0u);

//             // auto& data = get_struct_extra_data();
//             // assert(data.layoutElements.size() != 0u);

//             m_offset = Policy::advance_to_boundary(offset);
//             auto offset_next = *m_offset;

//             for(size_t i = 0; i < count; ++i)
//             {
//                 auto& elem = get_struct_element(i);

//                 offset_next = elem.template finalize<Policy>(offset_next);
//             }
        
//             return offset_next;
//         }

//         template<class Policy>
//         size_t finalize_for_array(size_t offset)
//         {
//             // auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
//             // assert( data.size != 0u );

//             // offset = advance_to_boundry(offset);
//             // data.layoutElements->finalize(*m_offset);

//             // data.element_size = Element::advance_to_boundry(data.layoutElements->get_size_in_bytes());
//             // return get_offset_end();

//             return 0u;
//         }


//         static Element& get_empty_element() noexcept
//         {
//             static Element empty{}; return empty;
//         }

//         static size_t advance_to_boundry(size_t offset)                      noexcept;
//         static size_t advance_if_crosses_boundry(size_t offset, size_t size) noexcept;
//         static bool   crosses_boundry(size_t offset, size_t size)            noexcept;
//         static bool   validate_symbol_name(const std::string& name)          noexcept;


//     protected:
//         ElementType                    m_type        = Empty;
//     private:
//         std::optional<size_t>          m_offset;
        
//         std::unique_ptr<ExtraDataBase> m_pExtraData;
//     };

//     class Layout
//     {
//         friend class LayoutCodex;
//         friend class Buffer;
//     public:
//         size_t      get_size_in_bytes() const noexcept;
//         std::string get_sig()           const noexcept;
//     protected:
//         Layout(std::shared_ptr<Element> root) noexcept;
//     protected:
//         std::shared_ptr<Element> m_root;
//     };


//     class RawLayout : public Layout
//     {
//         friend class LayoutCodex;
//     public:
//         RawLayout() noexcept;

//         Element& operator[](const std::string& key) noexcept;

//         template<ElementType type>
//         Element& add(const std::string& key) noexcept { return m_root->add<type>(key); }
//     private:
//         void                     clear_root()   noexcept;

//         template<class Policy>
//         std::shared_ptr<Element> deliver_root() noexcept
//         {
//             auto temp = std::move(m_root);
//             temp -> finalize<Policy>(0);
//             *this = RawLayout();
//             return std::move(temp);        
//         }
//     };

//     class CookedLayout : public Layout
//     {
//         friend class Buffer;
//         friend class LayoutCodex;
//     public:
//         const Element& operator[](const std::string& key) const noexcept;

//         std::shared_ptr<Element> share_root() const noexcept;
//     private:
//         CookedLayout(std::shared_ptr<Element> root) noexcept;
//         std::shared_ptr<Element> relinquish_root() const noexcept;
//     };

//     class ConstElementRef
//     {
//         friend class Buffer;
//         friend class ElementRef;
//     public:
//         class Ptr
//         {
//             friend class ConstElementRef;
//         public:
//             template<typename T>
//             operator const T*() const noexcept
//             {
//                 static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used for pointer conversion");
//                 return &static_cast<const T&>(*ref);
//             }
//         private:
//             Ptr(const ConstElementRef* ref) noexcept;
//         private:
//             const ConstElementRef* ref;
//         };
//     public:
//         bool exists() const noexcept;

//         ConstElementRef operator[](const std::string& key) const noexcept;
//         ConstElementRef operator[](size_t index)           const noexcept;

//         Ptr             operator&()                        const noexcept;

//         template<typename T>
//         operator const T&() const noexcept
//         {
//             static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used for conversion");
//             return *reinterpret_cast<const T*>(m_bytes + m_offset + m_layout->resolve<T>());
//         }
//     private:
//         ConstElementRef(const Element*, const char*, size_t) noexcept;
//     private:
//         size_t         m_offset;
//         const Element* m_layout;
//         const char*    m_bytes;
//     };

//     class ElementRef
//     {
//         friend class Buffer;
//     public:
//         class Ptr
//         {
//             friend class ElementRef;
//         public:
//             template<typename T>
//             operator const T*() const noexcept
//             {
//                 static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used for pointer conversion");
//                 return &static_cast<const T&>(*m_ref);
//             }
//         private:
//             Ptr(const ElementRef*) noexcept;
//         private:
//             const ElementRef* m_ref;
//         };
//     public:
//         operator ConstElementRef() const noexcept;

//         bool exists() const noexcept;

//         ElementRef operator[](const std::string& key) const noexcept;
//         ElementRef operator[](size_t index)           const noexcept;

//         template<typename T>
//         bool set_if_exits(const T& val) noexcept
//         {
//             if(exits()) { *this = val; return true; }
//             return false;
//         }

//         Ptr operator&() const noexcept;

//         template<typename T>
//         operator T&() const noexcept
//         {
//             static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in conversion");
//             return *reinterpret_cast<T*>(m_bytes + m_offset + m_layout->resolve<T>());
//         }

//         template<typename T>
//         T& operator=( const T& rhs ) const noexcept
//         {
//             static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in assignment");
//             return static_cast<T&>(*this) = rhs;
//         }
//     private:
//         ElementRef(const Element*, char*, size_t) noexcept;
//     private:
//         size_t         m_offset;
//         const Element* m_layout;
//         char*          m_bytes;
//     };


//     class LayoutCodex
//     {
//     public:
//         template <typename Policy> 
//         static CookedLayout resolve(RawLayout&& layout) noexcept
//         {
//             auto sig = layout.get_sig();
//             auto& map = get().m_map;
            
//             const auto i = map.find(sig);

//             if(i != map.end())
//             {
//                 layout.clear_root();
//                 return {i->second}; 
//             }

//             auto result = map.insert( {std::move(sig), layout.deliver_root<Policy>()} );

//             return {result.first->second};
//         }
//     private:
//         static LayoutCodex& get() noexcept;
//     private:
//         std::unordered_map<std::string, std::shared_ptr<Element>> m_map;
//     };


//     class Buffer
//     {
//     public:
//         // template<typename Policy>
//         // Buffer(RawLayout&& lay)         noexcept
//         // :Buffer(LayoutCodex::resolve<Policy>(std::move(lay)))
//         // {}

//         template<typename Policy>
//         static Buffer create(RawLayout&& lay) noexcept
//         {
//             return Buffer(LayoutCodex::resolve<Policy>(std::move(lay)));
//         }

//         Buffer(const CookedLayout&) noexcept;
//         Buffer(CookedLayout&&)      noexcept;
//         Buffer(const Buffer& )      noexcept;

//         Buffer(Buffer&&)            noexcept;

//         ElementRef      operator[](const std::string& key)       noexcept;
//         ConstElementRef operator[](const std::string& key) const noexcept;

//         const char*     get_data()         const noexcept;
//         size_t         get_size_in_bytes() const noexcept;
//         const Element& get_root_element()  const noexcept;

//         void copy_from(const Buffer&) noexcept;

//         std::shared_ptr<Element> share_root() const noexcept;

//     private:
//         std::shared_ptr<Element> m_pLayoutRoot;
//         std::vector<char>        m_bytes;
//     };


    
// }

// #ifndef DUS_IMPL_SOURCE
// #undef ELEMENT_TYPES
// #endif