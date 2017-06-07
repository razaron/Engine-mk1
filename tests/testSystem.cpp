#include <catch.hpp>

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

    TaskGraph& update(std::vector<Entity> *p_entities, double delta){
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

    TaskGraph& update(std::vector<Entity> *p_entities, double delta){
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

SCENARIO("Systems can manage their memory via their ObjectPool", "[system][objectpool]")
{
    GIVEN("A derived System, FooSystem")
    {
        FooSystem sys{};

        REQUIRE(sys.name == "FooSystem");

        WHEN("Adding objects to the System")
        {
            Handle i = sys.emplaceobject<int>(420);
            Handle a = sys.emplaceobject<std::array<std::size_t, 64>>(1u,2u,3u,4u,5u,6u,7u,8u,9u,10u);

            auto iPtr = sys.getobject<int>(i);
            auto aPtr = sys.getobject<std::array<std::size_t, 64>>(a);

            REQUIRE(*iPtr == 420);
            REQUIRE(*aPtr == (std::array<std::size_t, 64>{1u,2u,3u,4u,5u,6u,7u,8u,9u,10u}));

            THEN("Removing objects from the system")
            {
                sys.removeObject<std::array<std::size_t, 64>>(a);

                auto ptr = reinterpret_cast<Handle*>(aPtr);

                REQUIRE(ptr->isFree == true);
                REQUIRE(ptr->size == OBJECT_SIZE_64);
                REQUIRE(ptr->index == 1);
            }
        }
    }
}

SCENARIO("Systems can send and process Events from eachother", "[system][eventstream]")
{
    GIVEN("Two systems, FooSystem and BarSystem, with an empty initial state")
    {
        FooSystem foo{};
        BarSystem bar{};

        REQUIRE(foo.count == 0);

        WHEN("Running update loop 1, generating Events in BarSystem")
        {
            foo.update(nullptr, 0);
            bar.update(nullptr, 0);

            REQUIRE(foo.count == 0);

            THEN("Running update loop 2, propogating those events to FooSystem and processing them")
            {
                bar.propogateEvents(foo);

                foo.update(nullptr, 0);
                bar.update(nullptr, 0);

                REQUIRE(foo.count == 0+1+2+3+4);
            }
        }
    }
}
