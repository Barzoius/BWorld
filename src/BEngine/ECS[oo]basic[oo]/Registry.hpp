#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <string>
#include <cstdint>

using Entity = uint32_t;

struct Transform {
    float x, y, z;
};

struct Velocity {
    float x, y, z;
};

struct Name {
    std::string value;
};

struct Gravity{

};

class IComponentPool
{
public:
    virtual ~IComponentPool() = delete;
};

template <typename T>
class ComponentPool : IComponentPool
{
public:
    std:::unordered_map<Entity, T> m_components;
};


class Registry
{

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_pools;

    template<typename T>
    ComponentPool<T>& get_pool()
    {
        auto type = std::type_index(typeid(T));
        
        if(!m_pools.find(type) == m_pools.end())
            m_pools[type] = std::make_unique<ComponentPool<T>>();
           
        return *static_cast<ComponentPool<T>*>(m_pools[type].get());
    }

public:
    Entity create_entity()
    {
        static Entity next_id = 0;
        return next_id++;
    }

    template<typename T, typename... Args>
    T& add_component(Entity entity, Args&&... args) {
        auto& pool = get_pool<T>();
        return pool.m_components.emplace(
            entity, T{std::forward<Args>(args)...}
        ).first->second;
    }

    template<typename T>
    T* get_component(Entity entity) {
        auto& components = get_pool<T>().m_components;

        auto it = components.find(entity);

        return it != components.end() ? &it->second : nullptr;
    }

    template<typename T>
    void remove_component(Entity entity) {
        get_pool<T>().m_components.erase(entity);
    }
};