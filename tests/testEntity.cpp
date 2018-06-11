#include <catch.hpp>

#include "Entity.hpp"

using namespace razaron::core::component;
using namespace razaron::core::entity;

SCENARIO("Entities can be constructed from a ComponentMap, then be added/removed to/from", "[entity][component]")
{
    GIVEN("A ComponentMap")
    {
        ComponentMap map;
        Handle h1{ HandleSize{ 4 } };
        Handle h2{ HandleSize{ 4 } };
        Handle h3{ HandleSize{ 4 } };
        map[ComponentType::COMPONENT_1] = h1;
        map[ComponentType::COMPONENT_2] = h2;
        map[ComponentType::COMPONENT_3] = h3;

        REQUIRE(map[ComponentType::COMPONENT_1].index == h1.index);
        REQUIRE(map[ComponentType::COMPONENT_2].index == h2.index);
        REQUIRE(map[ComponentType::COMPONENT_3].index == h3.index);

        WHEN("An entity is constructed from the ComponentMap")
        {
            Entity e{map};

            REQUIRE(e[ComponentType::COMPONENT_1].index == h1.index);
            REQUIRE(e[ComponentType::COMPONENT_2].index == h2.index);
            REQUIRE(e[ComponentType::COMPONENT_3].index == h3.index);

            THEN("Adding ComponentHandles")
            {
                Handle h{ HandleSize{ 4 }};
                e.addComponent(ComponentHandle{ComponentType::COMPONENT_1, h});

                REQUIRE(e[ComponentType::COMPONENT_1].index == h.index);
            }

            THEN("Removing ComponentHandles")
            {
                Handle h{ HandleSize{ 4 } };
                e.removeComponent(ComponentHandle{ ComponentType::COMPONENT_1, h1 });

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
