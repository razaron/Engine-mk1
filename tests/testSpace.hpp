#pragma once

#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::system;
using namespace razaron::graph;
using namespace razaron;

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
    }

    ~SystemA() {}

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;

        return Task{};
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

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return Task{};
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        UNUSED(p_type);
        return ComponentHandle{};
    }
    bool removeComponent(ComponentHandle p_ch)
    {
        UNUSED(p_ch);
        return false;
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
    }
    ~SystemD() {}

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;

        return Task{};
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

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return Task{};
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        UNUSED(p_type);
        return ComponentHandle{};
    }
    bool removeComponent(ComponentHandle p_ch)
    {
        UNUSED(p_ch);
        return false;
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

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return Task{};
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        UNUSED(p_type);
        return ComponentHandle{};
    }
    bool removeComponent(ComponentHandle p_ch)
    {
        UNUSED(p_ch);
        return false;
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

    Task update(EntityMap &p_entities, double delta)
    {
        UNUSED(p_entities);
        UNUSED(delta);
        count++;
        return Task{};
    }

    ComponentHandle createComponent(ComponentType p_type)
    {
        UNUSED(p_type);
        return ComponentHandle{};
    }
    bool removeComponent(ComponentHandle p_ch)
    {
        UNUSED(p_ch);
        return false;
    }

    int count{};

  private:
    std::string sysName{"SystemF"};
};
