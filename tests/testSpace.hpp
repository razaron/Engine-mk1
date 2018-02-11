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
        _componentTypes.insert(ComponentType::COMPONENT_1);
    }

    ~SystemA() {}

    Task update(EntityMap &, double)
    {
        count++;

        return Task{};
    }

    ComponentHandle createComponent(ComponentType type, std::shared_ptr<void>)
    {
        Handle h;

        switch (type)
        {
        case ComponentType::COMPONENT_1:
            h = emplaceObject<Foo>(Foo{});
            break;
        default:
            h = Handle{};
            break;
        }

        return ComponentHandle{ type, h };
    }

    bool removeComponent(ComponentHandle ch)
    {
        Handle h;

        switch (ch.first)
        {
        case ComponentType::COMPONENT_1:
            removeObject<Foo>(ch.second);
            break;
        default:
            return false;
            break;
        }

        return true;
    }

    int count{};

  private:
    std::string sysName{ "SystemA" };
};

class SystemC : public System
{
  public:
    SystemC() {}
    ~SystemC() {}

	Task update(EntityMap &, double)
	{
		count++;
		return Task{};
	}

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemC" };
};

class SystemD : public System
{
  public:
    SystemD()
    {
        _componentTypes.insert(ComponentType::COMPONENT_2);
    }
    ~SystemD() {}

    Task update(EntityMap &, double)
    {
        count++;

        return Task{};
    }

    ComponentHandle createComponent(ComponentType type, std::shared_ptr<void>)
    {
        Handle h;

        switch (type)
        {
        case ComponentType::COMPONENT_2:
            h = emplaceObject<Bar>(Bar{});
            break;
        default:
            h = Handle{};
            break;
        }

        return ComponentHandle{ type, h };
    }

    bool removeComponent(ComponentHandle ch)
    {
        Handle h;

        switch (ch.first)
        {
        case ComponentType::COMPONENT_2:
            removeObject<Bar>(ch.second);
            break;
        default:
            return false;
            break;
        }

        return true;
    }

    int count{};

  private:
    std::string sysName{ "SystemD" };
};

class SystemB : public System
{
  public:
    SystemB() {}
    ~SystemB() {}

	Task update(EntityMap &, double)
	{
		count++;
		return Task{};
	}

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemB" };
};

class SystemE : public System
{
  public:
    SystemE() {}
    ~SystemE() {}

	Task update(EntityMap &, double)
	{
		count++;
		return Task{};
	}

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemE" };
};

class SystemF : public System
{
  public:
    SystemF() {}
    ~SystemF() {}

    Task update(EntityMap &, double)
    {
        count++;
        return Task{};
    }

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemF" };
};
