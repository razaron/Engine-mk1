#include <catch.hpp>

#include "testSpace.hpp"

SCENARIO("Spaces manage Systems and Events, updating them in BF order", "[space][system][graph][eventstream]")
{
    GIVEN("A space with some systems")
    {
        auto a = std::make_shared<SystemA>();
        auto b = std::make_shared<SystemB>();
        auto c = std::make_shared<SystemC>();
        auto d = std::make_shared<SystemD>();
        auto e = std::make_shared<SystemE>();
        auto f = std::make_shared<SystemF>();

        SystemGraph g;

        // clang-format off
		enum V { A, B, C, D, E, F, G, H };

        // clang-format on

        g.addEdge(V::A, V::B);
        g.addEdge(V::A, V::C);
        g.addEdge(V::B, V::D);
        g.addEdge(V::C, V::E);
        g.addEdge(V::D, V::F);
        g.addEdge(V::E, V::F);

		g[V::A].data = a;
		g[V::B].data = b;
		g[V::C].data = c;
		g[V::D].data = d;
		g[V::E].data = e;
		g[V::F].data = f;

        Space s{g};

        WHEN("Updating the Space to update the Systems")
        {
            s.update(0);

            REQUIRE(a->count == 1);
            REQUIRE(b->count == 1);
            REQUIRE(c->count == 1);
            REQUIRE(d->count == 1);
            REQUIRE(e->count == 1);
            REQUIRE(f->count == 1);

            THEN("Updating the space with delta > _maxInterval")
            {
                s.update(0.05 * 1.5);

                REQUIRE(a->count == 3);
                REQUIRE(b->count == 3);
                REQUIRE(c->count == 3);
                REQUIRE(d->count == 3);
                REQUIRE(e->count == 3);
                REQUIRE(f->count == 3);
            }
        }
    }
}

SCENARIO("Spaces can add/remove enitities, generating relavant components in their Systems", "[space][graph][system][entity][eventstream][component][objectpool]")
{
    GIVEN("A space with some systems")
    {
		auto a = std::make_shared<SystemA>();
		auto b = std::make_shared<SystemB>();
		auto c = std::make_shared<SystemC>();
		auto d = std::make_shared<SystemD>();
		auto e = std::make_shared<SystemE>();
		auto f = std::make_shared<SystemF>();

		SystemGraph g;

		// clang-format off
		enum V { A, B, C, D, E, F, G, H };

		// clang-format on

		g.addEdge(V::A, V::B);
		g.addEdge(V::A, V::C);
		g.addEdge(V::B, V::D);
		g.addEdge(V::C, V::E);
		g.addEdge(V::D, V::F);
		g.addEdge(V::E, V::F);

		g[V::A].data = a;
		g[V::B].data = b;
		g[V::C].data = c;
		g[V::D].data = d;
		g[V::E].data = e;
		g[V::F].data = f;

        Space s{g};

        WHEN("Creating new Entities (Foo+Bar)")
        {
            std::vector<Event> events;

            for(auto i=0;i<10;i++)
            {
                Event e{
					UUID64{0}, // Entity ID. 0 because unneeded
                    EventType::CREATE_ENTITY, // Event type enum
                    std::make_shared<core::eventdata::CREATE_ENTITY>(
                        std::list<ComponentArgs>{
                            ComponentArgs{ComponentType::COMPONENT_1, nullptr},
                            ComponentArgs{ComponentType::COMPONENT_2, nullptr}
                        }
                    )
                };

                events.push_back(e);
            }

            s.pushEvents(events, StreamType::INCOMING);

            for(auto i=0;i<100;i++)
            {
                s.update(0);
            }

            REQUIRE(s.getEntities().size() == 10);

            THEN("Removing the entity")
            {
                std::vector<Event> events;

                for(auto& [id, entity] : s.getEntities())
                {
					UNUSED(entity);

                    Event e{
                        id, // Entity ID.
                        EventType::REMOVE_ENTITY, // Event type enum
                        std::make_shared<core::eventdata::REMOVE_ENTITY>()
                    };

                    events.push_back(e);
                }

                s.pushEvents(events, StreamType::INCOMING);

                for(auto i=0;i<10;i++)
                {
                    s.update(0);
                }

                REQUIRE(s.getEntities().size() == 0);
            }
        }
    }
}
