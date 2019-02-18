#include "EventStream.hpp"

#include <catch.hpp>

#include <array>
#include <thread>

using namespace rz::eventstream;

SCENARIO("You can push/pop Events from an EventStream", "[eventstream]")
{
    GIVEN("An EventStream with no incoming and some outgoing events")
    {
        EventStream stream;

        stream.pushEvent({ UUID64{ 0 }, EventType::EVENT_1, std::make_shared<std::string>("lolwut") }, StreamType::OUTGOING);

        std::vector<Event> events;
        for (unsigned int i = 1; i < 5; i++)
        {
            Event e{ UUID64{ 0 }, EventType::EVENT_2, std::make_shared<std::string>("dis gun be gud") };
            events.push_back(e);
        }

        stream.pushEvents(events, StreamType::OUTGOING);

        WHEN("Popping outgoing events")
        {
            std::vector<Event> events = stream.popEvents(StreamType::OUTGOING);

            REQUIRE(events.size() == 5);
            REQUIRE(*(std::static_pointer_cast<std::string>(events[0].data)) == "lolwut");
            REQUIRE(*(std::static_pointer_cast<std::string>(events[1].data)) == "dis gun be gud");
        }

        WHEN("Events are pushed onto the incoming stream")
        {
            for (unsigned int i = 1; i < 5; i++)
            {
                stream.pushEvent({ UUID64{ 0 }, EventType::EVENT_2, std::make_shared<std::string>("get in dere") }, StreamType::INCOMING);
            }

            std::vector<Event> events = stream.popEvents(StreamType::INCOMING);

            REQUIRE(events.size() == 4);
            REQUIRE(*(std::static_pointer_cast<std::string>(events[0].data)) == "get in dere");
        }
    }
}

SCENARIO("EventStreams can propogate Events to eachother", "[eventstream]")
{
    GIVEN("An EventStream with outgoing events and an empty EventStream")
    {
        EventStream a;
        EventStream b;

        a.pushEvent({ UUID64{ 0 }, EventType::EVENT_1, std::make_shared<std::string>("lolwut") }, StreamType::OUTGOING);

        std::vector<Event> events;
        for (unsigned int i = 1; i < 5; i++)
        {
            Event e{ UUID64{ 0 }, EventType::EVENT_2, std::make_shared<std::string>("dis gun be gud") };
            events.push_back(e);
        }

        a.pushEvents(events, StreamType::OUTGOING);

        WHEN("a pushes outgoing Events to b")
        {
            auto events = a.popEvents(StreamType::OUTGOING);
            b.pushEvents(events, StreamType::INCOMING);

            events = b.popEvents(StreamType::INCOMING);

            REQUIRE(events.size() == 5);
            REQUIRE(*(std::static_pointer_cast<std::string>(events[0].data)) == "lolwut");
            REQUIRE(*(std::static_pointer_cast<std::string>(events[1].data)) == "dis gun be gud");
        }
    }
}

SCENARIO("An EventStream can be used from multiple threads", "[eventstream][concurrent]")
{
    GIVEN("A single EventStream, 1 producer thread and 1 consumer thread")
    {
        EventStream stream;
        std::vector<Event> popped;

        std::thread producer;
        std::thread consumer;

        WHEN("Pushing and popping events from the same StreamType")
        {
            std::vector<Event> events;
            for (auto i = 0u; i < 1000; i++)
            {
                events.push_back(Event{ UUID64{ 0 }, EventType::EVENT_2, {} });
            }

            THEN("If the StreamType is INCOMING")
            {
                producer = std::thread{ [&stream, &events]() {
                    stream.pushEvents(events, StreamType::INCOMING);
                } };

                consumer = std::thread{ [&stream, &popped]() {
                    auto vec = stream.popEvents(StreamType::INCOMING);
                    popped.insert(popped.end(), vec.begin(), vec.end());
                } };

                producer.join();
                consumer.join();

                auto result = stream.popEvents(StreamType::INCOMING);

                REQUIRE(result.size() + popped.size() == 1000);
            }

            THEN("If the StreamType is OUTGOING")
            {
                producer = std::thread{ [&stream, &events]() {
                    stream.pushEvents(events, StreamType::OUTGOING);
                } };

                consumer = std::thread{ [&stream, &popped]() {
                    auto vec = stream.popEvents(StreamType::OUTGOING);
                    popped.insert(popped.end(), vec.begin(), vec.end());
                } };

                producer.join();
                consumer.join();

                auto result = stream.popEvents(StreamType::OUTGOING);

                REQUIRE(result.size() + popped.size() == 1000);
            }
        }

        WHEN("Pushing and popping events from different StreamTypes")
        {
            std::vector<Event> events;
            for (auto i = 0u; i < 1000; i++)
            {
                events.push_back(Event{ UUID64{ 0 }, EventType::EVENT_2, {} });
            }

            THEN("If pushing to INCOMING and popping from OUTGOING")
            {
                producer = std::thread{ [&stream, &events]() {
                    stream.pushEvents(events, StreamType::INCOMING);
                } };

                consumer = std::thread{ [&stream]() {
                    stream.popEvents(StreamType::OUTGOING);
                } };

                producer.join();
                consumer.join();

                auto result1 = stream.popEvents(StreamType::INCOMING);
                auto result2 = stream.popEvents(StreamType::OUTGOING);

                REQUIRE(result1.size() == 1000);
                REQUIRE(result2.size() == 0);
            }

            THEN("If pushing to OUTGOING and popping from INCOMING")
            {
                producer = std::thread{ [&stream, &events]() {
                    stream.pushEvents(events, StreamType::OUTGOING);
                } };

                consumer = std::thread{ [&stream]() {
                    stream.popEvents(StreamType::INCOMING);
                } };

                producer.join();
                consumer.join();

                auto result1 = stream.popEvents(StreamType::OUTGOING);
                auto result2 = stream.popEvents(StreamType::INCOMING);

                REQUIRE(result1.size() == 1000);
                REQUIRE(result2.size() == 0);
            }
        }
    }

    GIVEN("A single EventStream, 4 producer threads and 4 consumer thread")
    {
        EventStream stream;
        std::vector<Event> popped;
        std::mutex poppedMutex;

        std::array<std::thread, 4> producer;
        std::array<std::thread, 4> consumer;

        WHEN("Pushing and popping events from the same StreamType")
        {
            std::vector<Event> events;
            for (auto i = 0u; i < 1000; i++)
            {
                events.push_back(Event{ UUID64{ 0 }, EventType::EVENT_2, {} });
            }

            THEN("If the StreamType is INCOMING")
            {
                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i] = std::thread{ [&stream, &events]() {
                        stream.pushEvents(events, StreamType::INCOMING);
                    } };
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i] = std::thread{ [&stream, &popped, &poppedMutex]() {
                        auto vec = stream.popEvents(StreamType::INCOMING);

                        std::lock_guard<std::mutex> lk{ poppedMutex };
                        popped.insert(popped.end(), vec.begin(), vec.end());
                    } };
                }

                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i].join();
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i].join();
                }

                auto result = stream.popEvents(StreamType::INCOMING);

                REQUIRE(result.size() + popped.size() == 4000);
            }

            THEN("If the StreamType is OUTGOING")
            {
                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i] = std::thread{ [&stream, &events]() {
                        stream.pushEvents(events, StreamType::OUTGOING);
                    } };
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i] = std::thread{ [&stream, &popped, &poppedMutex]() {
                        auto vec = stream.popEvents(StreamType::OUTGOING);

                        std::lock_guard<std::mutex> lk{ poppedMutex };
                        popped.insert(popped.end(), vec.begin(), vec.end());
                    } };
                }

                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i].join();
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i].join();
                }

                auto result = stream.popEvents(StreamType::OUTGOING);

                REQUIRE(result.size() + popped.size() == 4000);
            }
        }

        WHEN("Pushing and popping events from different StreamTypes")
        {
            std::vector<Event> events;
            for (auto i = 0u; i < 1000; i++)
            {
                events.push_back(Event{ UUID64{ 0 }, EventType::EVENT_2, {} });
            }

            THEN("If pushing to INCOMING and popping from OUTGOING")
            {
                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i] = std::thread{ [&stream, &events]() {
                        stream.pushEvents(events, StreamType::INCOMING);
                    } };
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i] = std::thread{ [&stream, &popped, &poppedMutex]() {
                        auto vec = stream.popEvents(StreamType::OUTGOING);

                        std::lock_guard<std::mutex> lk{ poppedMutex };
                        popped.insert(popped.end(), vec.begin(), vec.end());
                    } };
                }

                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i].join();
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i].join();
                }

                auto result1 = stream.popEvents(StreamType::INCOMING);
                auto result2 = stream.popEvents(StreamType::OUTGOING);

                REQUIRE(result1.size() == 4000);
                REQUIRE(result2.size() == 0);
            }

            THEN("If pushing to OUTGOING and popping from INCOMING")
            {
                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i] = std::thread{ [&stream, &events]() {
                        stream.pushEvents(events, StreamType::OUTGOING);
                    } };
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i] = std::thread{ [&stream, &popped, &poppedMutex]() {
                        auto vec = stream.popEvents(StreamType::INCOMING);

                        std::lock_guard<std::mutex> lk{ poppedMutex };
                        popped.insert(popped.end(), vec.begin(), vec.end());
                    } };
                }

                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i].join();
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i].join();
                }

                auto result1 = stream.popEvents(StreamType::OUTGOING);
                auto result2 = stream.popEvents(StreamType::INCOMING);

                REQUIRE(result1.size() == 4000);
                REQUIRE(result2.size() == 0);
            }

            THEN("If pushing and popping from both StreamTypes")
            {
                std::vector<Event> poppedIn;
                std::vector<Event> poppedOut;

                std::mutex poppedInMutex;
                std::mutex poppedOutMutex;

                for (unsigned i = 0; i < producer.size(); i++)
                {
                    if (i < 2)
                    {
                        producer[i] = std::thread{ [&stream, &events]() {
                            stream.pushEvents(events, StreamType::INCOMING);
                        } };
                    }
                    else
                    {
                        producer[i] = std::thread{ [&stream, &events]() {
                            stream.pushEvents(events, StreamType::OUTGOING);
                        } };
                    }
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    if (i < 2)
                    {
                        consumer[i] = std::thread{ [&stream, &poppedIn, &poppedInMutex]() {
                            auto vec = stream.popEvents(StreamType::INCOMING);

                            std::lock_guard<std::mutex> lk{ poppedInMutex };
                            poppedIn.insert(poppedIn.end(), vec.begin(), vec.end());
                        } };
                    }
                    else
                    {
                        consumer[i] = std::thread{ [&stream, &poppedOut, &poppedOutMutex]() {
                            auto vec = stream.popEvents(StreamType::OUTGOING);

                            std::lock_guard<std::mutex> lk{ poppedOutMutex };
                            poppedOut.insert(poppedOut.end(), vec.begin(), vec.end());
                        } };
                    }
                }

                for (unsigned i = 0; i < producer.size(); i++)
                {
                    producer[i].join();
                }

                for (unsigned i = 0; i < consumer.size(); i++)
                {
                    consumer[i].join();
                }

                auto resultIn = stream.popEvents(StreamType::INCOMING);
                auto resultOut = stream.popEvents(StreamType::OUTGOING);

                REQUIRE(resultIn.size() + poppedIn.size() == 2000);
                REQUIRE(resultOut.size() + poppedOut.size() == 2000);
            }
        }
    }
}
