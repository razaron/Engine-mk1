#ifndef RZ_TESTS_TESTSPACE_HPP
#define RZ_TESTS_TESTSPACE_HPP

#include "Space.hpp"

#include <functional>

class Foo : public rz::core::Component
{
};

class Bar : public rz::core::Component
{
};

class SystemA : public rz::core::System
{
  public:
    SystemA()
    {
        _componentTypes.insert(rz::core::ComponentType{ "FOO" });
    }

    ~SystemA() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;

        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType type, std::shared_ptr<void>)
    {
        Handle h{};

        if (type == std::string{ "FOO" })
            h = emplaceObject<Foo>();

        return rz::core::ComponentHandle{ type, h };
    }

    bool removeComponent(rz::core::ComponentHandle ch)
    {
        if (ch.first == std::string{ "FOO" })
            removeObject<Foo>(ch.second);
        else
            return false;

        return true;
    }

    int count{};

  private:
    std::string sysName{ "SystemA" };
};

class SystemC : public rz::core::System
{
  public:
    SystemC() {}
    ~SystemC() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;
        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return {}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemC" };
};

class SystemD : public rz::core::System
{
  public:
    SystemD()
    {
        _componentTypes.insert(rz::core::ComponentType{ "BAR" });
    }
    ~SystemD() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;

        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType type, std::shared_ptr<void>)
    {
        Handle h{};

        if (type == std::string{ "BAR" })
            h = emplaceObject<Bar>();

        return rz::core::ComponentHandle{ type, h };
    }

    bool removeComponent(rz::core::ComponentHandle ch)
    {
        if (ch.first == std::string{ "BAR" })
            removeObject<Bar>(ch.second);
        else
            return false;

        return true;
    }

    int count{};

  private:
    std::string sysName{ "SystemD" };
};

class SystemB : public rz::core::System
{
  public:
    SystemB() {}
    ~SystemB() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;
        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return {}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemB" };
};

class SystemE : public rz::core::System
{
  public:
    SystemE() {}
    ~SystemE() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;
        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return {}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemE" };
};

class SystemF : public rz::core::System
{
  public:
    SystemF() {}
    ~SystemF() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;
        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return {}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemF" };
};

#endif //RZ_TESTS_TESTSPACE_HPP
