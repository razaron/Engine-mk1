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

    TaskGraph& update(EntityMap &p_entities, double delta){
        UNUSED(p_entities);
        UNUSED(delta);

        auto events = popEvents();

        for(auto &e : events)
        {
            if(e.type == EventType::TYPE_1)
            {
                int *data = static_cast<int*>(e.data);

                count += *data;

                delete data;
            }
        }

        return m_taskGraph;
    }

    int count{};
    std::string name { "FooSystem" };
};

class BarSystem : public System
{
  public:
    BarSystem() {}
    ~BarSystem() {}

    TaskGraph& update(EntityMap &p_entities, double delta){
        UNUSED(p_entities);
        UNUSED(delta);

        for(auto i=0;i<5;i++)
        {
            pushEvent(Event{0, EventType::TYPE_1, new int{i}});
        }

        return m_taskGraph;
    }

    std::string name { "BarSystem" };
};
