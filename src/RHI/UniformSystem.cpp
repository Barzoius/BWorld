#define DUS_IMPL_SOURCE

#include "UniformSystem.hpp"

#include <algorithm>

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
    size_t Element::get_offset_end()    const noexcept
    {
        switch( m_type)
        {
        #define X(elem) case elem: return *m_offset + Map<elem>::shader_size;
        ELEMENT_TYPES
        #undef X
        case Struct: return 0u;
        case Array:  return 0u;
        default: assert("Attempt retrival of offset for empty or invlaid type" && false); return 0u;
        }
    
    }
    size_t Element::get_size_in_bytes() const noexcept
    {
        return get_offset_end() - get_offset_begin();
    }

    Element& Element::add(ElementType type, std::string name) noexcept
    {

    }


    Element& Element::set(ElementType type, size_t size)      noexcept
    {

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

    size_t Element::finalize(size_t offset) noexcept
    {
        switch( m_type )
        {
        #define X(elem) case elem: m_offset = 0u; return 0u; /// based on policy
        ELEMENT_TYPES
        #undef X
        case Struct: return finalize_for_struct(offset);
        case Array : return finalize_for_array(offset);
        default    : assert("Attempt finalization on bad type" && false); return 0u;
        }
    }


    std::string Element::get_sig_for_struct() const noexcept
    {

    }
    std::string Element::get_sig_for_array()  const noexcept
    {

    }

    size_t Element::finalize_for_struct(size_t offset)
    {

    }
    size_t Element::finalize_for_array(size_t offset)
    {

    }




    size_t Element::advance_to_boundry(size_t offset)                      noexcept
    {

    }
    size_t Element::advance_if_crosses_boundry(size_t offset, size_t size) noexcept
    {

    }
    bool   Element::crosses_boundry(size_t offset, size_t size)            noexcept
    {

    }
    bool   Element::validate_symbol_name(const std::string& name)          noexcept
    {
        return !name.empty() && !std::isdigit(name.front()) &&
        std::all_of(name.begin(), name.end(), [](char c){return std::isalnum(c) || c == '_';});
    }


    ///==================================[LAYOUT]==================================///

    Layout::Layout(std::shared_ptr<Element> root) noexcept : m_root(std::move(root)){}

    size_t      Layout::get_size_in_bytes() const noexcept { return m_root->get_size_in_bytes(); }
    std::string Layout::get_sig()           const noexcept { return m_root->get_sig(); }

    ///================================[RAW LAYOUT]================================///

    RawLayout::RawLayout() noexcept : Layout(std::shared_ptr<Element>{new Element{ Struct }}) {}

    Element& RawLayout::operator[](const std::string& key) noexcept { return (*m_root)[key]; }

    void                     RawLayout::clear_root()   noexcept { *this = RawLayout(); }
    std::shared_ptr<Element> RawLayout::deliver_root() noexcept
    {
        auto temp = std::move(m_root);
        temp -> finalize(0);
        *this = RawLayout();
        return std::move(temp);
    }

    ///==============================[FINISHED LAYOUT]=============================///

    CookedLayout::CookedLayout(std::shared_ptr<Element> root) noexcept : Layout(std::move(root)) {}

    const Element& CookedLayout::operator[](const std::string& key) const noexcept { return (*m_root)[key]; }

    std::shared_ptr<Element> CookedLayout::share_root() const noexcept { return m_root; }
    

    std::shared_ptr<Element> CookedLayout::relinquish_root() const noexcept{ return std::move(m_root); }

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

    Buffer::Buffer(RawLayout&&)             noexcept
    {

    }

    Buffer::Buffer(const CookedLayout& lay) noexcept 
    : m_pLayoutRoot(lay.share_root()), m_bytes(m_pLayoutRoot->get_offset_end())
    {}

    Buffer::Buffer(CookedLayout&& lay)      noexcept
    : m_pLayoutRoot(lay.relinquish_root()), m_bytes(m_pLayoutRoot->get_offset_end())
    {}

    Buffer::Buffer(const Buffer& buf)       noexcept
    : m_pLayoutRoot(buf.m_pLayoutRoot), m_bytes(buf.m_bytes)
    {}

    Buffer::Buffer(Buffer&& buf)            noexcept
    : m_pLayoutRoot(std::move(buf.m_pLayoutRoot)), m_bytes(std::move(buf.m_bytes))
    {}

    ElementRef      Buffer::operator[](const std::string& key)       noexcept
    {
        return { &(*m_pLayoutRoot)[key], m_bytes.data(), 0u };
    }

    ConstElementRef Buffer::operator[](const std::string& key) const noexcept
    {
        return const_cast<Buffer&>(*this)[key];
    }

    const char*     Buffer::get_data()         const noexcept { return m_bytes.data(); }

    size_t         Buffer::get_size_in_bytes() const noexcept { return m_bytes.size(); }

    const Element& Buffer::get_root_element()  const noexcept { return *m_pLayoutRoot; }

    void Buffer::copy_from(const Buffer& other) noexcept
    {
        assert(&get_root_element() == &other.get_root_element());
        std::copy(other.m_bytes.begin(), other.m_bytes.end(), m_bytes.begin());
    }

    std::shared_ptr<Element> Buffer::share_root() const noexcept { return m_pLayoutRoot; }

}