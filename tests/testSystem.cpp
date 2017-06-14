#include <catch.hpp>

#include "testSystem.hpp"

SCENARIO("Systems can manage their memory via their ObjectPool", "[system][objectpool]")
{
    GIVEN("A derived System, FooSystem")
    {
        FooSystem sys{};

        REQUIRE(sys.name == "FooSystem");

        WHEN("Adding objects to the System")
        {
            Handle i = sys.emplaceObject<int>(420);
            Handle a = sys.emplaceObject<std::array<std::size_t, 64>>(1u,2u,3u,4u,5u,6u,7u,8u,9u,10u);

            auto iPtr = sys.getObject<int>(i);
            auto aPtr = sys.getObject<std::array<std::size_t, 64>>(a);

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

        EntityMap eMap{};

        WHEN("Running update loop 1, generating Events in BarSystem")
        {
            foo.update(eMap, 0);
            bar.update(eMap, 0);

            REQUIRE(foo.count == 0);

            THEN("Running update loop 2, propogating those events to FooSystem and processing them")
            {
                bar.propogateEvents(foo);

                foo.update(eMap, 0);
                bar.update(eMap, 0);

                REQUIRE(foo.count == 0+1+2+3+4);
            }
        }
    }
}
