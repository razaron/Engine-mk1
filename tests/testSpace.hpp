#pragma once

#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::system;
using namespace razaron::graph;

namespace eventdata
{
    struct CREATE_COMPONENT
    {
        ComponentType type;

        CREATE_COMPONENT(ComponentType p_type) : type{p_type} {}
    };
    struct ADD_COMPONENT
    {
        ComponentHandle ch;

        ADD_COMPONENT(ComponentHandle p_ch) : ch{p_ch} {}
    };
    struct REMOVE_COMPONENT
    {
        ComponentHandle ch;
        bool isRemoved{false};

        REMOVE_COMPONENT(ComponentHandle p_ch) : ch{p_ch} {}
        REMOVE_COMPONENT(ComponentHandle p_ch, bool p_isRemoved) : ch{p_ch}, isRemoved{p_isRemoved} {}
    };
}

class Foo : public Component
{
};

class Bar : public Component
{
};

class SystemA : public System
{
  public:
    SystemA()
    {
        m_componentTypes.insert(ComponentType::FOO);

        registerHandler(EventType::CREATE_COMPONENT, [system = this](Event & e) {
            auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

            // If ComponentType is valid, do something
            if (system->m_componentTypes.count(data->type))
            {
                auto ch = system->createComponent(data->type);

                system->pushEvent(Event{
                    e.recipient,
                    EventType::ADD_COMPONENT,
                    std::make_shared<eventdata::ADD_COMPONENT>(ch)
                });
            }
        });

        registerHandler(EventType::REMOVE_COMPONENT, [system = this](Event & e) {
            auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

            // If ComponentType is valid, do something
            if (system->m_componentTypes.count(data->ch.first))
            {
                bool result = system->removeComponent(data->ch);

                system->pushEvent(Event{
                    e.recipient,
                    EventType::REMOVE_COMPONENT,
                    std::make_shared<eventdata::REMOVE_COMPONENT>(data->ch, result)
                });
            }
        });
    }

    ~SystemA() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;

        if (count == 1)
        {
            pushEvent(Event{0u, EventType::CREATE_ENTITY, std::make_shared<std::string>("Foo + Bar")});
        }

        return m_taskGraph;
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        Handle h;

        switch (p_type)
        {
        case ComponentType::FOO:
            h = emplaceObject<Foo>(Foo{});
            break;
        default:
            h = Handle{};
            break;
        }

        return ComponentHandle{p_type, h};
    }

    bool removeComponent(ComponentHandle p_ch)
    {
        Handle h;

        switch (p_ch.first)
        {
        case ComponentType::FOO:
            removeObject<Foo>(p_ch.second);
            break;
        default:
            return false;
            break;
        }

        return true;
    }

    int count{};

  private:
    std::string sysName{"SystemA"};
};

class SystemC : public System
{
  public:
    SystemC() {}
    ~SystemC() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return m_taskGraph;
    }

    int count{};

  private:
    std::string sysName{"SystemC"};
};

class SystemD : public System
{
  public:
    SystemD()
    {
        m_componentTypes.insert(ComponentType::BAR);

        registerHandler(EventType::CREATE_COMPONENT, [system = this](Event & e) {
            auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

            // If ComponentType is valid, do something
            if (system->m_componentTypes.count(data->type))
            {
                auto ch = system->createComponent(data->type);

                system->pushEvent(Event{
                    e.recipient,
                    EventType::ADD_COMPONENT,
                    std::make_shared<eventdata::ADD_COMPONENT>(ch)
                });
            }
        });

        registerHandler(EventType::REMOVE_COMPONENT, [system = this](Event & e) {
            auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

            // If ComponentType is valid, do something
            if (system->m_componentTypes.count(data->ch.first))
            {
                bool result = system->removeComponent(data->ch);

                system->pushEvent(Event{
                    e.recipient,
                    EventType::REMOVE_COMPONENT,
                    std::make_shared<eventdata::REMOVE_COMPONENT>(data->ch, result)
                });
            }
        });
    }
    ~SystemD() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;

        return m_taskGraph;
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        Handle h;

        switch (p_type)
        {
        case ComponentType::BAR:
            h = emplaceObject<Bar>(Bar{});
            break;
        default:
            h = Handle{};
            break;
        }

        return ComponentHandle{p_type, h};
    }

    bool removeComponent(ComponentHandle p_ch)
    {
        Handle h;

        switch (p_ch.first)
        {
        case ComponentType::BAR:
            removeObject<Bar>(p_ch.second);
            break;
        default:
            return false;
            break;
        }

        return true;
    }

    int count{};

  private:
    std::string sysName{"SystemD"};
};

class SystemB : public System
{
  public:
    SystemB() {}
    ~SystemB() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return m_taskGraph;
    }

    int count{};

  private:
    std::string sysName{"SystemB"};
};

class SystemE : public System
{
  public:
    SystemE() {}
    ~SystemE() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return m_taskGraph;
    }

    int count{};

  private:
    std::string sysName{"SystemE"};
};

class SystemF : public System
{
  public:
    SystemF() {}
    ~SystemF() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return m_taskGraph;
    }

    int count{};

  private:
    std::string sysName{"SystemF"};
};
