#include "EventStream.hpp"

#include "catch.hpp"

using namespace razaron::eventstream;

SCENARIO("You can push/pop Events from an EventStream")
{
    GIVEN("An EventStream with no incoming and some outgoing events")
    {
        EventStream stream;

        stream.pushEvent({0, EventType::TYPE_1, new std::string{"lolwut"}}, StreamType::OUTGOING);

        std::vector<Event> events;
        for (unsigned int i = 1; i < 5; i++)
        {
            Event e{i, EventType::TYPE_2, new std::string{"dis gun be gud"}};
            events.push_back(e);
        }

        stream.pushEvents(events, StreamType::OUTGOING);

        WHEN("Popping outgoing events")
        {
            std::vector<Event> events = stream.popEvents(StreamType::OUTGOING);

            REQUIRE(events.size() == 5);
            REQUIRE(*(static_cast<std::string *>(events[0].data)) == "lolwut");
            REQUIRE(*(static_cast<std::string *>(events[1].data)) == "dis gun be gud");
        }

        WHEN("Events are pushed onto the incoming stream")
        {
            for (unsigned int i = 1; i < 5; i++)
            {
                stream.pushEvent({i, EventType::TYPE_2, new std::string{"get in dere"}}, StreamType::INCOMING);
            }

            std::vector<Event> events = stream.popEvents(StreamType::INCOMING);

            REQUIRE(events.size() == 4);
            REQUIRE(*(static_cast<std::string *>(events[0].data)) == "get in dere");
        }
    }
}

SCENARIO("EventStream objects can bubble/capture Events from eachother")
{
    GIVEN("Two EventStreams, a and b, with some outgoing Events")
    {
        EventStream a;
        EventStream b;

        a.pushEvent({0, EventType::TYPE_1, new std::string{"lolwut"}}, StreamType::OUTGOING);
        b.pushEvent({0, EventType::TYPE_2, new std::string{"lolwut"}}, StreamType::OUTGOING);

        std::vector<Event> eventsA;
        std::vector<Event> eventsB;
        for (unsigned int i = 1; i < 5; i++)
        {
            Event eA{i, EventType::TYPE_1, new std::string{"dis gun be gud"}};
            eventsA.push_back(eA);

            Event eB{i, EventType::TYPE_2, new std::string{"dis gun be gud"}};
            eventsB.push_back(eB);
        }

        a.pushEvents(eventsA, StreamType::OUTGOING);
        b.pushEvents(eventsB, StreamType::OUTGOING);

        WHEN("a bubbles Events to b")
        {
            a.bubbleEvents(&b);

            std::vector<Event> events = b.popEvents(StreamType::INCOMING);

            REQUIRE(events.size() == 5);
            REQUIRE(*(static_cast<std::string *>(events[0].data)) == "lolwut");
            REQUIRE(*(static_cast<std::string *>(events[1].data)) == "dis gun be gud");
        }

        WHEN("a captures events from b")
        {
            a.captureEvents(&b);

            std::vector<Event> events = a.popEvents(StreamType::INCOMING);

            REQUIRE(events.size() == 5);
            REQUIRE(*(static_cast<std::string *>(events[0].data)) == "lolwut");
            REQUIRE(*(static_cast<std::string *>(events[1].data)) == "dis gun be gud");
        }
    }
}
