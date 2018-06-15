#ifndef RZ_TESTS_TESTSYSTEM_H
#define RZ_TESTS_TESTSYSTEM_H

#include "System.hpp"

class FooSystem : public rz::core::System
{
  public:
    FooSystem()
    {
        extendHandler(rz::eventstream::EventType::EVENT_1, [&](const rz::eventstream::Event &e) {
            auto data = std::static_pointer_cast<int>(e.data);

            count += *data;
        });
    }

    ~FooSystem() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return rz::core::ComponentHandle{}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    int count{};
    std::string name{ "FooSystem" };
};

class BarSystem : public rz::core::System
{
  public:
    BarSystem() {}
    ~BarSystem() {}

    rz::taskscheduler::Task update(rz::core::EntityMap &, double)
    {
        for (auto i = 0; i < 5; i++)
        {
            _eventStream.pushEvent(rz::eventstream::Event{ UUID64{ 0 }, rz::eventstream::EventType::EVENT_1, std::make_shared<int>(i) }, rz::eventstream::StreamType::OUTGOING);
        }

        return rz::taskscheduler::Task{};
    }

    rz::core::ComponentHandle createComponent(rz::core::ComponentType, std::shared_ptr<void>) { return rz::core::ComponentHandle{}; }
    bool removeComponent(rz::core::ComponentHandle) { return false; }

    std::string name{ "BarSystem" };
};

#endif //RZ_TESTS_TESTSYSTEM_H
