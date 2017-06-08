#include <catch.hpp>

#include "Entity.hpp"

using namespace razaron::core::component;
using namespace razaron::core::entity;

SCENARIO("Entities can be constructed from a ComponentMap, then be added/removed to/from", "[entity][component]")
{
    GIVEN("A ComponentMap")
    {
        ComponentMap map;
        map[ComponentType::FOO] = Handle{HandleSize{4}, HandleIndex{0}, false};
        map[ComponentType::BAR] = Handle{HandleSize{4}, HandleIndex{1}, false};
        map[ComponentType::COLLIDER] = Handle{HandleSize{4}, HandleIndex{2}, false};

        REQUIRE(map[ComponentType::FOO].index == 0);
        REQUIRE(map[ComponentType::BAR].index == 1);
        REQUIRE(map[ComponentType::COLLIDER].index == 2);

        WHEN("An entity is constructed from the ComponentMap")
        {
            Entity e{map};

            REQUIRE(e[ComponentType::FOO].index == 0);
            REQUIRE(e[ComponentType::BAR].index == 1);
            REQUIRE(e[ComponentType::COLLIDER].index == 2);

            THEN("Adding ComponentHandles")
            {
                e.addComponent(ComponentHandle{ComponentType::CONTROLLER, Handle{HandleSize{4}, HandleIndex{3}, false}});

                REQUIRE(e[ComponentType::CONTROLLER].index == 3);
            }

            THEN("Removing ComponentHandles")
            {
                e.removeComponent(ComponentHandle{ComponentType::FOO, Handle{HandleSize{4}, HandleIndex{0}, false}});

                bool result = false;

                try
                {
                    e[ComponentType::FOO];
                }
                catch (const std::invalid_argument& e)
                {
                    result = true;
                }
                
                REQUIRE(result == true);
            }
        }
    }
}
