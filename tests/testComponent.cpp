#include "Component.hpp"

#include <catch.hpp>

#include <set>
#include <vector>

using namespace rz::core;

SCENARIO("Components are generated with unique IDs", "[component]")
{
    GIVEN("A std::vector of Components")
    {
        std::vector<Component> v;

        for (auto i = 0; i < 10; i++)
        {
            Component c;
            v.push_back(c);
        }

        REQUIRE(v.size() == 10);

        THEN("Each will have a unique ID")
        {
            std::set<UUID64, UUID64Cmp> s;

            for (std::size_t i = 0; i < v.size(); i++)
            {
                s.insert(v[i].getID());
            }

            REQUIRE(s.size() == v.size());
        }
    }
}

SCENARIO("Classes deriving from class Component are generated with unique IDs", "[component]")
{
    class Foo : public Component
    {
      public:
        Foo(){};
        ~Foo(){};
    };

    GIVEN("A std::vector of Components")
    {
        std::vector<Foo> v;

        for (auto i = 0; i < 10; i++)
        {
            Foo f;
            v.push_back(f);
        }

        REQUIRE(v.size() == 10);

        THEN("Each will have a unique ID")
        {
            std::set<UUID64, UUID64Cmp> s;

            for (std::size_t i = 0; i < v.size(); i++)
            {
                s.insert(v[i].getID());
            }

            REQUIRE(s.size() == v.size());
        }
    }
}
