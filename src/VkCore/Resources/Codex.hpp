#include <memory>
#include <type_traits>
#include <unordered_map>
#include <string>


template<typename T>
struct ResourceTraits;

struct VertexBufferDesc
{
    size_t size;
    uint32_t stride;
        uint32_t usage;

    std::string get_UID() const
    {

    };

};

struct VertexBuffer {};

template<>
struct ResourceTraits<VertexBuffer>
{
    using Desc = VertexBufferDesc;
};


struct ShaderModule {};

struct ShaderDesc
{
    std::string path;

    std::string get_UID() const
    {

    };
};

template<>
struct ResourceTraits<ShaderModule>
{
    using Desc = ShaderDesc;
};


template<class Resource>
class Codex
{
public:
    using Desc = typename ResourceTraits<Resource>::Desc;

    template<typename...Params>
    static std::shared_ptr<Resource> resolve(const Desc& desc, Params&&...p) noexcept
    {
        return resolve_<Resource>(std::forward<Params>(p));
    }

private:
    template<typename...Params>
    std::shared_ptr<Resource> resolve_(const Desc& desc, Params&&...p) noexcept
    {
        const auto key = desc.get_UID;
        const auto exits = m_cache.find(key);

        if(exits == m_cache.end())
        {
            auto res = std::make_shared<Resource>();
            m_cache[key] = res;
            return res;
        }
        else
        {
            return std::static_pointer_cast<Resource>(exits->second);
        }
    } 


private:
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_cache;
};