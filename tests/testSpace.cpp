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

    // Starts the chain of events that will create a new Entity
    s.registerHandler(EventType::CREATE_ENTITY, [space = &s](Event &e) {
        auto data = static_cast<std::string*>(e.data);
        std::cout << "\tCREATE_ENTITY  -  " << *data << std::endl;

        auto entity = space->createEntity();

        std::vector<Event> events;

        events.push_back(Event{
            entity.getID(),
            EventType::CREATE_COMPONENT,
            new eventdata::CREATE_COMPONENT{ComponentType::FOO}
        });

        events.push_back(Event{
            entity.getID(),
            EventType::CREATE_COMPONENT,
            new eventdata::CREATE_COMPONENT{ComponentType::BAR}
        });

        space->pushEvents(events, StreamType::OUTGOING);

        delete data;
    });

    // Adds a created Component to the correct entity
    s.registerHandler(EventType::ADD_COMPONENT, [space = &s](Event &e) {
        auto data = static_cast<eventdata::ADD_COMPONENT*>(e.data);

        std::cout << "\tADD_COMPONENT - " << ((data->ch.first == ComponentType::FOO) ? "Foo" : "Bar") << std::endl;

        if(!data->ch.second.isFree)
        {
            std::cout << "\tSuccess" << std::endl;

            auto ent = (*space)[e.recipient];
            ent.addComponent(data->ch);
        }
        
        delete data;
    });

    for(auto i=0;i<10;i++)
    {
        std::cout << "Update " << i << ":" << std::endl;
        s.update(0);
    }
}
