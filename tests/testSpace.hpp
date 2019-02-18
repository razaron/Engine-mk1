#ifndef RZ_TESTS_TESTSPACE_HPP
#define RZ_TESTS_TESTSPACE_HPP

#include "Space.hpp"

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
        _componentTypes.insert(rz::core::ComponentType::COMPONENT_1);
    }

    ~SystemA() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;

        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType type, std::shared_ptr<void>)
    {
        Handle h;

        switch (type)
        {
            case rz::core::ComponentType::COMPONENT_1:
                h = emplaceObject<Foo>();
                break;
            default:
                h = Handle{};
                break;
        }

        return rz::core::ComponentHandle{ type, h };
    }

    bool removeComponent(rz::core::ComponentHandle ch)
    {
        switch (ch.first)
        {
            case rz::core::ComponentType::COMPONENT_1:
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

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return rz::core::ComponentHandle{}; }
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
        _componentTypes.insert(rz::core::ComponentType::COMPONENT_2);
    }
    ~SystemD() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        count++;

        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType type, std::shared_ptr<void>)
    {
        Handle h;

        switch (type)
        {
            case rz::core::ComponentType::COMPONENT_2:
                h = emplaceObject<Bar>();
                break;
            default:
                h = Handle{};
                break;
        }

        return rz::core::ComponentHandle{ type, h };
    }

    bool removeComponent(rz::core::ComponentHandle ch)
    {
        switch (ch.first)
        {
            case rz::core::ComponentType::COMPONENT_2:
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

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return rz::core::ComponentHandle{}; }
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

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return rz::core::ComponentHandle{}; }
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

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return rz::core::ComponentHandle{}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    int count{};

  private:
    std::string sysName{ "SystemF" };
};

#endif //RZ_TESTS_TESTSPACE_HPP
