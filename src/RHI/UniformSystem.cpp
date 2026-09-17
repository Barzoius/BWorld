#define DUS_IMPL_SOURCE

// #include "UniformSystem.hpp"

#include "Test.hpp"

#include <algorithm>
#include <cctype>
#include <string>


#include <iostream>

namespace DUS
{

    struct ExtraData
    {
        struct Struct : public Element::ExtraDataBase
        {
            std::vector<std::pair<std::string, Element>> layoutElements;
        };

        struct Array : public Element::ExtraDataBase
        {
            std::optional<Element> layoutElements;
            size_t                 element_size;
            size_t                 size;
        };
    };


    ///==================================[ELEMENT]==================================///

    size_t  Element::get_struct_element_count() const
    {
        auto& data = static_cast<ExtraData::Struct&>(*m_pExtraData);
        return data.layoutElements.size(); 
    }

    Element& Element::get_struct_element(size_t index) const
    {
        auto& data = static_cast<ExtraData::Struct&>(*m_pExtraData);
        return data.layoutElements[index].second;
    }

    std::string Element::get_sig() const noexcept
    {
        switch( m_type )
        {
        #define X(elem) case elem: return Map<elem>::code;
        ELEMENT_TYPES
        #undef X
        case Struct: return get_sig_for_struct();
        case Array : return get_sig_for_array();
        default    : assert("Unknown type in sig gen" && false); return "failed sig";
        }
    }

    bool Element::exists() const noexcept { return m_type != Empty; }

    std::pair<size_t, const Element*> Element::calc_index_offset(size_t offset, size_t index) const noexcept
    {
        assert("Attempt index accesing in non array type" && m_type == Array);
        const auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
        assert( index < data.size );
        return { offset + data.element_size * index, &*data.layoutElements };
    }

    Element& Element::operator[](const std::string& key) noexcept
    {
        assert("Attempting to key access in non struct" && m_type == Struct);

        for(auto& mem : static_cast<ExtraData::Struct&>(*m_pExtraData).layoutElements)
            if(mem.first == key) return mem.second;

        return get_empty_element();
    }

    const Element& Element::operator[](const std::string& key) const noexcept
    {
        return const_cast<Element&>(*this)[key];
    }

    Element& Element::T() noexcept
    {
        assert("Attempting access T of non array type" && m_type == Array);
        return *static_cast<ExtraData::Array&>(*m_pExtraData).layoutElements;
    }
    const Element& Element::T() const noexcept
    {
        return const_cast<Element&>(*this).T();
    }


    size_t Element::get_offset_begin()  const noexcept
    {
        return *m_offset;
    }
    // size_t Element::get_offset_end()    const noexcept
    // {
    //     switch( m_type)
    //     {
    //     #define X(elem) case elem: return *m_offset + Map<elem>::shader_size;
    //     ELEMENT_TYPES
    //     #undef X
    //     case Struct: return 0u;
    //     case Array:  return 0u;
    //     default: assert("Attempt retrival of offset for empty or invlaid type" && false); return 0u;
    //     }
    
    // }
    // size_t Element::get_size_in_bytes() const noexcept
    // {
    //     return get_offset_end() - get_offset_begin();
    // }

    Element& Element::add(ElementType type, std::string name) noexcept
    {
        assert("Attempt to add to non struct layout" && m_type == Struct); 
        assert("Invalid symbol name in Struct type" && validate_symbol_name(name));

        auto& structData = static_cast<ExtraData::Struct&>(*m_pExtraData);

        for(auto& mem : structData.layoutElements)
            if(mem.first == name) assert("Attemot to add duplicate name to struct" && false);
		
        structData.layoutElements.emplace_back(std::move(name), Element{type});
        return *this; 
    }


    Element& Element::set(ElementType type, size_t size)      noexcept
    {
        assert("Attempt to set on non array in layout" && type == Array);
        assert( size != 0u );

        auto& arrayData = static_cast<ExtraData::Array&>(*m_pExtraData);
        arrayData.layoutElements = {type};
        arrayData.size = size;
        
        return *this;
    }

    Element::Element(ElementType type) noexcept : m_type(type)
    {
        assert(type != Empty);
        if(type == Struct)
        {
            m_pExtraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
        }
        else if(type == Array)
        {
            m_pExtraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
        }
    }

    // size_t Element::finalize(size_t offset) noexcept
    // {
    //     switch( m_type )
    //     {
    //     #define X(elem) case elem: m_offset = advance_if_crosses_boundry(offset ,Map<elem>::shader_size); return *m_offset + Map<elem>::shader_size;
    //     ELEMENT_TYPES
    //     #undef X
    //     case Struct: return finalize_for_struct(offset);
    //     case Array : return finalize_for_array(offset);
    //     default    : assert("Attempt finalization on bad type" && false); return 0u;
    //     }
    // }


    std::string Element::get_sig_for_struct() const noexcept
    {
        using namespace std::string_literals;
        auto sig = "St{"s;

        for(const auto& elem : static_cast<ExtraData::Struct&>(*m_pExtraData).layoutElements)
            sig += elem.first + ":"s + elem.second.get_sig() + ";"s;
        
        sig += "}"s;
        return sig;
    }

    std::string Element::get_sig_for_array()  const noexcept
    {
        using namespace std::string_literals;
        const auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
        return "Ar:"s + std::to_string( data.size ) + "{"s + data.layoutElements->get_sig() + "}"s;
    }

    // size_t Element::finalize_for_struct(size_t offset)
    // {
    //     auto& data = static_cast<ExtraData::Struct&>(*m_pExtraData);
    //     assert(data.layoutElements.size() != 0u);

    //     m_offset = advance_to_boundry(offset);
    //     auto offset_next = *m_offset;

    //     for(auto& elem : data.layoutElements)
    //         offset_next = elem.second.finalize(offset_next);
        
    //     return offset_next;
    // }

    // size_t Element::finalize_for_array(size_t offset)
    // {
    //     auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
    //     assert( data.size != 0u );

    //     offset = advance_to_boundry(offset);
    //     data.layoutElements->finalize(*m_offset);

    //     data.element_size = Element::advance_to_boundry(data.layoutElements->get_size_in_bytes());
    //     return get_offset_end();
    // }




    size_t Element::advance_to_boundry(size_t offset)                      noexcept
    {
        return 0u;
    }
    size_t Element::advance_if_crosses_boundry(size_t offset, size_t size) noexcept
    {
        return 0u;

    }
    bool   Element::crosses_boundry(size_t offset, size_t size)            noexcept
    {
        return 0u;

    }
    bool   Element::validate_symbol_name(const std::string& name)          noexcept
    {
        return !name.empty() && !std::isdigit(name.front()) &&
        std::all_of(name.begin(), name.end(), [](char c){return std::isalnum(c) || c == '_';});
    }


    ///==================================[LAYOUT]==================================///

    Layout::Layout(std::shared_ptr<Element> root) noexcept : m_root(std::move(root)){}

    // size_t      Layout::get_size_in_bytes() const noexcept { return m_root->get_size_in_bytes(); }
    std::string Layout::get_sig()           const noexcept { return m_root->get_sig(); }

    ///================================[RAW LAYOUT]================================///

    RawLayout::RawLayout() noexcept : Layout(std::shared_ptr<Element>{new Element{ Struct }}) {}

    Element& RawLayout::operator[](const std::string& key) noexcept { return (*m_root)[key]; }

    void RawLayout::clear_root()   noexcept { *this = RawLayout(); }

    ///==============================[FINISHED LAYOUT]=============================///

    // template<typename Policy>
    // CookedLayout<Policy>::CookedLayout(std::shared_ptr<Element> root) noexcept : Layout(std::move(root)) {}

    template<typename Policy>
    const Element& CookedLayout<Policy>::operator[](const std::string& key) const noexcept { return (*m_root)[key]; }

    // template<typename Policy>
    // std::shared_ptr<Element> CookedLayout<Policy>::share_root() const noexcept { return m_root; }
    
    // template<typename Policy>
    // std::shared_ptr<Element> CookedLayout<Policy>::relinquish_root() const noexcept{ return std::move(m_root); }

    ///============================[CONST ELEMENT REF]=============================///

    ConstElementRef::ConstElementRef(const Element* lay, const char* bytes, size_t offset) noexcept
    : m_layout(lay), m_bytes(bytes), m_offset(offset) {}

    ConstElementRef::Ptr::Ptr(const ConstElementRef* ref) noexcept : ref(ref) {}

    bool ConstElementRef::exists() const noexcept { return m_layout->exists(); }

    ConstElementRef ConstElementRef::operator[](const std::string& key) const noexcept
    {
        return {&(*m_layout)[key], m_bytes, m_offset};
    }
    ConstElementRef ConstElementRef::operator[](size_t index)           const noexcept
    {
        const auto indexedData = m_layout->calc_index_offset(m_offset, index);
        return {indexedData.second, m_bytes, indexedData.first };
    }

    ConstElementRef::Ptr ConstElementRef::operator&() const noexcept{ return Ptr{this}; }


    ///===============================[ELEMENT REF]================================///

    ElementRef::ElementRef( const Element* lay ,char* bytes, size_t offset ) noexcept
    : m_offset(offset), m_layout(lay), m_bytes(bytes){}


    ElementRef::Ptr::Ptr(const ElementRef* ref) noexcept : m_ref(ref) {}

    ElementRef::operator ConstElementRef() const noexcept { return { m_layout, m_bytes, m_offset }; }

    bool ElementRef::exists() const noexcept { return m_layout -> exists(); }

    ElementRef ElementRef::operator[]( const std::string& key ) const noexcept
    {
        return { &(*m_layout)[key], m_bytes, m_offset };
    }

    ElementRef ElementRef::operator[]( size_t index ) const noexcept
    {
        const auto indexedData = m_layout->calc_index_offset(m_offset, index);
        return { indexedData.second, m_bytes, indexedData.first };
    }

    ElementRef::Ptr ElementRef::operator&() const noexcept 
    {
        return Ptr{ const_cast<ElementRef*>(this) };
    }

    ///==================================[BUFFER]==================================///

    
    ///==================================[CODEX]===================================///
   
    LayoutCodex& LayoutCodex::get() noexcept
    {
        static LayoutCodex codex;
        return codex;
    }



}//END::DUS

