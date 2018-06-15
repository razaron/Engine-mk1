#include "Entity.hpp"

#include <catch.hpp>

using namespace rz::core;

SCENARIO("Entities can be constructed from a ComponentMap, then be added/removed to/from", "[entity][component]")
{
    GIVEN("A ComponentMap")
    {
        ComponentMap map;
        Handle h1{ HandleType{ 4 } };
        Handle h2{ HandleType{ 4 } };
        Handle h3{ HandleType{ 4 } };
        map[ComponentType::COMPONENT_1] = h1;
        map[ComponentType::COMPONENT_2] = h2;
        map[ComponentType::COMPONENT_3] = h3;

        REQUIRE(map[ComponentType::COMPONENT_1].id == h1.id);
        REQUIRE(map[ComponentType::COMPONENT_2].id == h2.id);
        REQUIRE(map[ComponentType::COMPONENT_3].id == h3.id);

        WHEN("An entity is constructed from the ComponentMap")
        {
            Entity e{ map };

            REQUIRE(e[ComponentType::COMPONENT_1].id == h1.id);
            REQUIRE(e[ComponentType::COMPONENT_2].id == h2.id);
            REQUIRE(e[ComponentType::COMPONENT_3].id == h3.id);

            THEN("Adding ComponentHandles")
            {
                Handle h{ HandleType{ 4 } };
                e.addComponent(ComponentHandle{ ComponentType::COMPONENT_1, h });

                REQUIRE(e[ComponentType::COMPONENT_1].id == h.id);
            }

            THEN("Removing ComponentHandles")
            {
                Handle h{ HandleType{ 4 } };
                e.removeComponent(ComponentHandle{ ComponentType::COMPONENT_1, h1 });

                bool result = false;

                // COMPONENT_1 removed so access should throw
                try
                {
                    e[ComponentType::COMPONENT_1];
                }
                catch (const std::invalid_argument &)
                {
                    result = true;
                }

                REQUIRE(result == true);
            }
        }
    }
}
