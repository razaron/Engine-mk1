#pragma once

#include "System.hpp"

using namespace razaron::core::system;
using namespace razaron::core::component;
using namespace razaron::core::entity;
using namespace razaron::graph;

class FooSystem : public System
{
  public:
    FooSystem() {}
    ~FooSystem() {}

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);

        auto events = popEvents();

        for (auto &e : events)
        {
            if (e.type == EventType::REMOVE_COMPONENT)
            {
                auto data = std::static_pointer_cast<int>(e.data);

                count += *data;
            }
        }

        return Task{};
    }

    ComponentHandle createComponent(ComponentType p_type, std::shared_ptr<void> p_tuplePtr) { UNUSED(p_type); return ComponentHandle{}; }
    bool removeComponent(ComponentHandle p_ch) { UNUSED(p_ch); return false; }

    int count{};
    std::string name{"FooSystem"};
};

class BarSystem : public System
{
  public:
    BarSystem() {}
    ~BarSystem() {}

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);

        for (auto i = 0; i < 5; i++)
        {
            pushEvent(Event{0, EventType::REMOVE_COMPONENT, std::make_shared<int>(i)});
        }

        return Task{};
    }

    ComponentHandle createComponent(ComponentType p_type, std::shared_ptr<void> p_tuplePtr) { UNUSED(p_type); return ComponentHandle{}; }
    bool removeComponent(ComponentHandle p_ch) { UNUSED(p_ch); return false; }

    std::string name{"BarSystem"};
};
