#include <catch.hpp>

#include "testSpace.hpp"

SCENARIO("Spaces manage Systems and Events, updating them in BF order", "[space][system][graph][eventstream]")
{
    GIVEN("A space with some systems")
    {
        auto a = new SystemA{};
        auto b = new SystemB{};
        auto c = new SystemC{};
        auto d = new SystemD{};
        auto e = new SystemE{};
        auto f = new SystemF{};

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

            THEN("Updating the space with delta > m_maxInterval")
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
        auto a = new SystemA{};
        auto b = new SystemB{};
        auto c = new SystemC{};
        auto d = new SystemD{};
        auto e = new SystemE{};
        auto f = new SystemF{};

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

        WHEN("Creating a new Entity (Foo+Bar)")
        {
            unsigned int entityID = 0;

            // Starts the chain of events that will create a new Entity
            s.registerHandler(EventType::CREATE_ENTITY, [ space = &s, &entityID ](Event & e) {
                auto data = std::static_pointer_cast<std::string>(e.data);

                auto entity = space->createEntity();

                entityID = entity.getID();

                std::vector<Event> events;

                events.push_back(Event{
                    entity.getID(),
                    EventType::CREATE_COMPONENT,
                    std::make_shared<eventdata::CREATE_COMPONENT>(ComponentType::FOO)}
                );

                events.push_back(Event{
                    entity.getID(),
                    EventType::CREATE_COMPONENT,
                    std::make_shared<eventdata::CREATE_COMPONENT>(ComponentType::BAR)}
                );

                space->pushEvents(events, StreamType::OUTGOING);
            });

            int count = 2;

            // Adds a created Component to the correct entity
            s.registerHandler(EventType::ADD_COMPONENT, [ space = &s, &count ](Event & e) {
                auto data = std::static_pointer_cast<eventdata::ADD_COMPONENT>(e.data);

                if (!data->ch.second.isFree)
                {
                    count--;

                    (*space)[e.recipient].addComponent(data->ch);
                }
            });

            for (auto i = 0; i < 4; i++)
            {
                s.update(0);
            }

            REQUIRE(count == 0);

            THEN("Removing the entity")
            {
                std::vector<Event> events;

                auto ptr1 = std::make_shared<eventdata::REMOVE_COMPONENT>(ComponentHandle{
                    ComponentType::FOO,
                    s[entityID][ComponentType::FOO]}
                );

                events.push_back(Event{
                    entityID,
                    EventType::REMOVE_COMPONENT,
                    ptr1}
                );

                auto ptr2 = std::make_shared<eventdata::REMOVE_COMPONENT>(ComponentHandle{
                    ComponentType::BAR,
                    s[entityID][ComponentType::FOO]}
                );
                events.push_back(Event{
                    entityID,
                    EventType::REMOVE_COMPONENT,
                    ptr2}
                );

                s.pushEvents(events, StreamType::OUTGOING);

                count = 999;
                // Adds a created Component to the correct entity
                s.registerHandler(EventType::REMOVE_COMPONENT, [ space = &s, &count ](Event & e) {
                    auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

                    if (data->isRemoved)
                    {
                        (*space)[e.recipient].removeComponent(data->ch);

                        if ((*space)[e.recipient].getComponents().size() == 0)
                            count = space->removeEntity(e.recipient);
                    }
                });

                for (auto i = 0; i < 4; i++)
                {
                    s.update(0);
                }

                REQUIRE(count == 0);
            }
        }
    }
}
