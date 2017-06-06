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

        return m_taskGraph;
    }

    std::string name { "FooSystem" };
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
