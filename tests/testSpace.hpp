#pragma once

#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::system;
using namespace razaron::graph;

namespace eventdata{
    struct CREATE_COMPONENT
    {
        ComponentType type;
    };
    struct ADD_COMPONENT
    {
        ComponentHandle ch;
    };
}

class Foo : public Component
{
  public:
    Foo(){};
    ~Foo(){};
};

class Bar : public Component
{
  public:
    Bar(){};
    ~Bar(){};
};

class SystemA : public System
{
  public:
    SystemA()
    {
        m_componentTypes.insert(ComponentType::FOO);

        registerHandler(EventType::CREATE_COMPONENT, [system = this](Event &e){
            auto data = static_cast<eventdata::CREATE_COMPONENT*>(e.data);

            // If ComponentType is valid, do something
            if(system->m_componentTypes.count(data->type))
            {
                std::cout << "\t\tCREATE_COMPONENT FOO" << std::endl;

                auto ch = system->createComponent(data->type);

                system->pushEvent(Event{
                    e.recipient,
                    EventType::ADD_COMPONENT,
                    new eventdata::ADD_COMPONENT{ch}
                });

                delete data;
            }
        });
    }

    ~SystemA() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities); UNUSED(delta); count++;

        if(count == 1)
        {
            pushEvent(Event{0u, EventType::CREATE_ENTITY, new std::string{"Foo + Bar"}});
        }

        return m_taskGraph;
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        Handle h;

        switch(p_type)
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

	int count = 0;

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
		UNUSED(p_entities); UNUSED(delta); count++;
        return m_taskGraph;
    }

	int count = 0;

  private:
    std::string sysName{"SystemC"};
};

class SystemD : public System
{
  public:
    SystemD()
    {
        m_componentTypes.insert(ComponentType::BAR);

        registerHandler(EventType::CREATE_COMPONENT, [system = this](Event &e){
            auto data = static_cast<eventdata::CREATE_COMPONENT*>(e.data);

            // If ComponentType is valid, do something
            if(system->m_componentTypes.count(data->type))
            {
                std::cout << "\t\tCREATE_COMPONENT BAR" << std::endl;

                auto ch = system->createComponent(data->type);

                system->pushEvent(Event{
                    e.recipient,
                    EventType::ADD_COMPONENT,
                    new eventdata::ADD_COMPONENT{ch}
                });

                delete data;
            }
        });
    }
    ~SystemD() {}

    TaskGraph &update(EntityMap &p_entities, double delta)
    {
		UNUSED(p_entities); UNUSED(delta); count++;
        return m_taskGraph;
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        Handle h;

        switch(p_type)
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

	int count = 0;

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
		UNUSED(p_entities); UNUSED(delta); count++;
        return m_taskGraph;
    }

	int count = 0;

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
		UNUSED(p_entities); UNUSED(delta); count++;
        return m_taskGraph;
    }

	int count = 0;

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
		UNUSED(p_entities); UNUSED(delta); count++;
        return m_taskGraph;
    }

	int count = 0;

  private:
    std::string sysName{"SystemF"};
};
