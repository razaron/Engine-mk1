#include "testSystem.hpp"

#include <catch.hpp>

using namespace rz::core;
using namespace rz::eventstream;

SCENARIO("Systems can manage their memory via their ObjectPool", "[system][objectpool]")
{
    GIVEN("A derived System, FooSystem")
    {
        FooSystem sys{};

        REQUIRE(sys.name == "FooSystem");

        WHEN("Adding objects to the System")
        {
            Handle i = sys.emplaceObject<std::array<int, 1>>(420);
            Handle a = sys.emplaceObject<std::array<std::size_t, 64>>(1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u);

            auto iPtr = sys.getObject<std::array<int, 1>>(i);
            auto aPtr = sys.getObject<std::array<std::size_t, 64>>(a);

            REQUIRE((*iPtr)[0] == 420);
            REQUIRE(*aPtr == (std::array<std::size_t, 64>{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u }));

            THEN("Removing objects from the system")
            {
                sys.removeObject<std::array<std::size_t, 64>>(a);

                bool result = false;
                try
                {
                    auto ptr = sys.getObject<std::array<std::size_t, 64>>(a);
                }
                catch (const rz::objectpool::error::HandleOutOfRange &)
                {
                    result = true;
                }

                REQUIRE(result == true);
            }
        }
    }
}

void loop(FooSystem &foo, BarSystem &bar)
{
    auto events = bar.popEvents(StreamType::OUTGOING);
    foo.pushEvents(events, StreamType::INCOMING);
    bar.pushEvents(events, StreamType::INCOMING);

    EntityMap map{};
    foo.update(map, 0);
    bar.update(map, 0);

    foo.processEvents();
    bar.processEvents();
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
            loop(foo, bar);

            REQUIRE(foo.count == 0);

            THEN("Running update loop 2, propogating those events to FooSystem and processing them")
            {
                loop(foo, bar);

                REQUIRE(foo.count == 0 + 1 + 2 + 3 + 4);
            }
        }
    }
}
