#include <catch.hpp>

#include "Entity.hpp"

using namespace razaron::core::component;
using namespace razaron::core::entity;

SCENARIO("Entities can be constructed from a ComponentMap, then be added/removed to/from", "[entity][component]")
{
    GIVEN("A ComponentMap")
    {
        ComponentMap map;
        map[ComponentType::COMPONENT_1] = Handle{HandleSize{4}, HandleIndex{0}, false};
        map[ComponentType::COMPONENT_2] = Handle{HandleSize{4}, HandleIndex{1}, false};
        map[ComponentType::COMPONENT_3] = Handle{HandleSize{4}, HandleIndex{2}, false};

        REQUIRE(map[ComponentType::COMPONENT_1].index == 0);
        REQUIRE(map[ComponentType::COMPONENT_2].index == 1);
        REQUIRE(map[ComponentType::COMPONENT_3].index == 2);

        WHEN("An entity is constructed from the ComponentMap")
        {
            Entity e{map};

            REQUIRE(e[ComponentType::COMPONENT_1].index == 0);
            REQUIRE(e[ComponentType::COMPONENT_2].index == 1);
            REQUIRE(e[ComponentType::COMPONENT_3].index == 2);

            THEN("Adding ComponentHandles")
            {
                e.addComponent(ComponentHandle{ComponentType::COMPONENT_1, Handle{HandleSize{4}, HandleIndex{3}, false}});

                REQUIRE(e[ComponentType::COMPONENT_1].index == 3);
            }

            THEN("Removing ComponentHandles")
            {
                e.removeComponent(ComponentHandle{ComponentType::COMPONENT_1, Handle{HandleSize{4}, HandleIndex{0}, false}});

                bool result = false;

				// COMPONENT_1 removed so access should throw
                try
                {
                    e[ComponentType::COMPONENT_1];
                }
                catch (const std::invalid_argument&)
                {
                    result = true;
                }

                REQUIRE(result == true);
            }
        }
    }
}
