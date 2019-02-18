#include "ObjectPool.hpp"

#include <catch.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace rz::objectpool;

SCENARIO("ObjectPools can allocate objects in multiple ways", "[objectpool]")
{
    GIVEN("An empty ObjectPool")
    {
        using Data = std::pair<int, float>;

        ObjectPool p;

        REQUIRE(p.capacity() == 0);

        WHEN("Constructing in-place")
        {
            Handle h = p.emplace<Data>(1, 2.f);

            REQUIRE(p.get<Data>(h)->first == 1);
            REQUIRE(p.get<Data>(h)->second == 2.f);
        }

        WHEN("Passing a const reference")
        {
            const Data data{ 1, 2.f };
            Handle h = p.push(data);

            REQUIRE(p.get<Data>(h)->first == 1);
            REQUIRE(p.get<Data>(h)->second == 2.f);
        }

        WHEN("Passing an rvalue")
        {
            Handle h = p.push(Data{ 1, 2.f });

            REQUIRE(p.get<Data>(h)->first == 1);
            REQUIRE(p.get<Data>(h)->second == 2.f);
        }
    }
}

SCENARIO("ObjectPools can add pages as necessary", "[objectpool]")
{
    GIVEN("An empty ObjectPool")
    {
        ObjectPool p;

        REQUIRE(p.capacity() == 0);

        WHEN("A single OBJECT_SIZE_2-byte object is pushed on")
        {
            p.push(std::array<char, OBJECT_SIZE_2>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_2-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.push(std::array<char, OBJECT_SIZE_2>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_4-byte object is pushed on")
        {
            p.push(std::array<char, OBJECT_SIZE_4>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_4-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.push(std::array<char, OBJECT_SIZE_4>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_8-byte object is pushed on")
        {
            p.push(std::array<char, OBJECT_SIZE_8>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_8-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.push(std::array<char, OBJECT_SIZE_8>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_16-byte object is pushed on")
        {
            p.push(std::array<char, OBJECT_SIZE_16>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_16-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.push(std::array<char, OBJECT_SIZE_16>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_32-byte object is pushed on")
        {
            p.push(std::array<char, OBJECT_SIZE_32>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_32-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.push(std::array<char, OBJECT_SIZE_32>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_64-byte object is pushed on")
        {
            p.push(std::array<char, OBJECT_SIZE_64>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_64);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_64-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.push(std::array<char, OBJECT_SIZE_64>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_64 * 2);
            }
        }
    }
}

SCENARIO("ObjectPools can retrieve objects from Handles", "[objectpool]")
{
    ObjectPool p;

    std::vector<Handle> h2;
    std::vector<Handle> h4;
    std::vector<Handle> h8;
    std::vector<Handle> h16;
    std::vector<Handle> h32;
    std::vector<Handle> h64;

    REQUIRE(p.capacity() == 0);

    GIVEN("An object pool with a SINGLE page of objects")
    {
        h2.push_back(p.push(std::array<char, OBJECT_SIZE_2>{ "first" }));
        h4.push_back(p.push(std::array<char, OBJECT_SIZE_4>{ "second" }));
        h8.push_back(p.push(std::array<char, OBJECT_SIZE_8>{ "third" }));
        h16.push_back(p.push(std::array<char, OBJECT_SIZE_16>{ "fourth" }));
        h32.push_back(p.push(std::array<char, OBJECT_SIZE_32>{ "fifth" }));
        h64.push_back(p.push(std::array<char, OBJECT_SIZE_64>{ "sixth" }));

        THEN("the handles can be converted to objects")
        {
            auto p2 = *p.get<std::array<char, OBJECT_SIZE_2>>(h2[0]);
            auto p4 = *p.get<std::array<char, OBJECT_SIZE_4>>(h4[0]);
            auto p8 = *p.get<std::array<char, OBJECT_SIZE_8>>(h8[0]);
            auto p16 = *p.get<std::array<char, OBJECT_SIZE_16>>(h16[0]);
            auto p32 = *p.get<std::array<char, OBJECT_SIZE_32>>(h32[0]);
            auto p64 = *p.get<std::array<char, OBJECT_SIZE_64>>(h64[0]);

            REQUIRE(p2[4] == 't');
            REQUIRE(p4[4] == 'n');
            REQUIRE(p8[4] == 'd');
            REQUIRE(p16[4] == 't');
            REQUIRE(p32[4] == 'h');
            REQUIRE(p64[2] == 'x');
        }
    }

    GIVEN("An object pool with MULTIPLE pages of objects")
    {
        for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH * 5; i++)
        {
            h2.push_back(p.push(std::array<int, OBJECT_SIZE_2 / sizeof(int)>{ 2 }));
            h4.push_back(p.push(std::array<int, OBJECT_SIZE_4 / sizeof(int)>{ 4 }));
            h8.push_back(p.push(std::array<int, OBJECT_SIZE_8 / sizeof(int)>{ 8 }));
            h16.push_back(p.push(std::array<int, OBJECT_SIZE_16 / sizeof(int)>{ 16 }));
            h32.push_back(p.push(std::array<int, OBJECT_SIZE_32 / sizeof(int)>{ 32 }));
            h64.push_back(p.push(std::array<int, OBJECT_SIZE_64 / sizeof(int)>{ 64 }));
        }

        THEN("the handles can be converted to objects")
        {
            bool result = true;

            for (HandleIndex i = 0; i < OBJECT_POOL_PAGE_LENGTH * 5; i++)
            {

                auto p2 = *p.get<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>(h2[i]);
                auto p4 = *p.get<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>(h4[i]);
                auto p8 = *p.get<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>(h8[i]);
                auto p16 = *p.get<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>(h16[i]);
                auto p32 = *p.get<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>(h32[i]);
                auto p64 = *p.get<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>(h64[i]);

                if (p2[0] != 2)
                    result = false;
                if (p2[1] != 0)
                    result = false;
                if (p4[0] != 4)
                    result = false;
                if (p4[1] != 0)
                    result = false;
                if (p8[0] != 8)
                    result = false;
                if (p8[1] != 0)
                    result = false;
                if (p16[0] != 16)
                    result = false;
                if (p16[1] != 0)
                    result = false;
                if (p32[0] != 32)
                    result = false;
                if (p32[1] != 0)
                    result = false;
                if (p64[0] != 64)
                    result = false;
                if (p64[1] != 0)
                    result = false;
            }

            REQUIRE(result == true);
        }
    }
}

typedef std::array<char, OBJECT_SIZE_2> Arr;

SCENARIO("You removes objects from anywhere in the ObjectPool", "[objectpool]")
{

    GIVEN("The ObjectPool {filled, filled, filled, empty...}")
    {
        ObjectPool pool;

        REQUIRE(pool.capacity() == 0);

        // Pushed into OBJECT_SIZE_2-byte pool
        auto first = pool.emplace<Arr>('f', 'i', 'r', 's', 't', '\0');
        auto p1 = pool.get<Arr>(first);

        // Pushed into OBJECT_SIZE_2-byte pool
        auto second = pool.emplace<Arr>('s', 'e', 'c', 'o', 'n', 'd', '\0');
        auto p2 = pool.get<Arr>(second);

        // Pushed into OBJECT_SIZE_2-byte pool
        auto third = pool.emplace<Arr>('t', 'h', 'i', 'r', 'd', '\0');
        auto p3 = pool.get<Arr>(third);

        REQUIRE(std::string{ p1->data() } == std::string{ "first" });
        REQUIRE(std::string{ p2->data() } == std::string{ "second" });
        REQUIRE(std::string{ p3->data() } == std::string{ "third" });

        WHEN("Removing an object from position 0")
        {
            pool.erase<Arr>(first);

            bool result = false;
            try
            {
                auto ptr = pool.get<Arr>(first);
            }
            catch (const error::HandleOutOfRange &)
            {
                result = true;
            }

            REQUIRE(result == true);

            THEN("Removing an object from position 2")
            {
                pool.erase<Arr>(third);

                bool result = false;
                try
                {
                    auto ptr = pool.get<Arr>(third);
                }
                catch (const error::HandleOutOfRange &)
                {
                    result = true;
                }

                REQUIRE(result == true);

                pool.emplace<Arr>('a', 'l', 'p', 'h', 'a', '\0');
                pool.emplace<Arr>('g', 'a', 'm', 'm', 'a', '\0');

                REQUIRE(std::string{ p1->data() } == std::string{ "alpha" });
                REQUIRE(std::string{ p2->data() } == std::string{ "second" });
                REQUIRE(std::string{ p3->data() } == std::string{ "gamma" });
            }
        }
    }
}

SCENARIO("ObjectPools can reorder objects to earlier free positions, then remove unnecessary pages", "[objectpool]")
{
    GIVEN("An object pool  with 2 empty pages followed by 2 full pages and 1 (reserved) empty page")
    {
        ObjectPool p;
        std::vector<Handle> v1, v2, v3, v4, v5, v6;

        // Creates 4 pages in every sub-pool
        for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH * 4; i++)
        {
            v1.push_back(p.push(std::array<int, OBJECT_SIZE_2 / sizeof(int)>{ 2 }));
            v2.push_back(p.push(std::array<int, OBJECT_SIZE_4 / sizeof(int)>{ 4 }));
            v3.push_back(p.push(std::array<int, OBJECT_SIZE_8 / sizeof(int)>{ 8 }));
            v4.push_back(p.push(std::array<int, OBJECT_SIZE_16 / sizeof(int)>{ 16 }));
            v5.push_back(p.push(std::array<int, OBJECT_SIZE_32 / sizeof(int)>{ 32 }));
            v6.push_back(p.push(std::array<int, OBJECT_SIZE_64 / sizeof(int)>{ 64 }));
        }

        std::size_t expected = 5 * OBJECT_POOL_PAGE_LENGTH * (OBJECT_SIZE_2 + OBJECT_SIZE_4 + OBJECT_SIZE_8 + OBJECT_SIZE_16 + OBJECT_SIZE_32 + OBJECT_SIZE_64);

        REQUIRE(expected == p.capacity());

        // Erases the first 2 pages from every sub-pool
        for (HandleIndex i = 0; i < OBJECT_POOL_PAGE_LENGTH * 2; i++)
        {
            p.erase<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>(v1.front());
            v1.erase(v1.begin());

            p.erase<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>(v2.front());
            v2.erase(v2.begin());

            p.erase<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>(v3.front());
            v3.erase(v3.begin());

            p.erase<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>(v4.front());
            v4.erase(v4.begin());

            p.erase<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>(v5.front());
            v5.erase(v5.begin());

            p.erase<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>(v6.front());
            v6.erase(v6.begin());
        }

        WHEN("The 2nd 2 pages have their objects reordered.")
        {
            // Reorders everything in the 2nd 2 pages of every sub-pool
            p.defragment();

            // Verifies that the 2nd 2 pages have been reordered
            bool result = true;
            for (HandleIndex i = 0; i < OBJECT_POOL_PAGE_LENGTH * 2; i++)
            {

                auto p2 = *p.get<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>(v1.front());
                v1.erase(v1.begin());

                auto p4 = *p.get<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>(v2.front());
                v2.erase(v2.begin());

                auto p8 = *p.get<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>(v3.front());
                v3.erase(v3.begin());

                auto p16 = *p.get<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>(v4.front());
                v4.erase(v4.begin());

                auto p32 = *p.get<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>(v5.front());
                v5.erase(v5.begin());

                auto p64 = *p.get<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>(v6.front());
                v6.erase(v6.begin());

                if (p2[0] != 2)
                    result = false;
                if (p2[1] != 0)
                    result = false;
                if (p4[0] != 4)
                    result = false;
                if (p4[1] != 0)
                    result = false;
                if (p8[0] != 8)
                    result = false;
                if (p8[1] != 0)
                    result = false;
                if (p16[0] != 16)
                    result = false;
                if (p16[1] != 0)
                    result = false;
                if (p32[0] != 32)
                    result = false;
                if (p32[1] != 0)
                    result = false;
                if (p64[0] != 64)
                    result = false;
                if (p64[1] != 0)
                    result = false;
            }

            REQUIRE(result == true);

            THEN("Shrinking the ObjectPool will release 3 pages worth of memory")
            {
                p.shrink();

                std::size_t expected = 2 * OBJECT_POOL_PAGE_LENGTH * (OBJECT_SIZE_2 + OBJECT_SIZE_4 + OBJECT_SIZE_8 + OBJECT_SIZE_16 + OBJECT_SIZE_32 + OBJECT_SIZE_64);

                REQUIRE(expected == p.capacity());
            }
        }
    }
}

SCENARIO("ObjectPools have helper functions to support automated lifetime management of objects", "[objectpool][lifetime]")
{
    ObjectPool pool;
    auto handle = pool.push(int{ 42 });

    WHEN("Using the helper function `makeUnique`")
    {
        auto unique = pool.makeUnique<int>(handle);

        THEN("Deleting the returned `std::unique_ptr` will automatically erase the object")
        {
            unique = nullptr;

            bool result = false;
            try
            {
                pool.get<int>(handle);
            }
            catch (const error::HandleOutOfRange &)
            {
                result = true;
            }

            REQUIRE(result == true);
        }
    }

    WHEN("Using the helper function `makeShared`")
    {
        auto shared = pool.makeShared<int>(handle);
        auto sharedCopy = shared;

        THEN("Deleting the returned `std::shared_ptr` and all copies will automatically erase the object")
        {
            shared = nullptr;
            sharedCopy = nullptr;

            bool result = false;
            try
            {
                pool.get<int>(handle);
            }
            catch (const error::HandleOutOfRange &)
            {
                result = true;
            }

            REQUIRE(result == true);
        }
    }
}

template <typename T, std::size_t N>
void pusher(ObjectPool &pool, T value, std::vector<Handle> &handles, std::mutex &mutex)
{
    for (auto i = 0; i < N; i++)
    {
        Handle h = pool.push(value);

        {
            std::lock_guard<std::mutex> lk{ mutex };
            handles.push_back(std::move(h));
        }
    }
}

template <typename T, std::size_t N>
void eraser(ObjectPool &pool, std::vector<Handle> &handles, std::mutex &mutex, std::atomic<unsigned> &counter)
{
    for (auto i = 0; i < N; i++)
    {
        Handle h{};

        {
            std::lock_guard<std::mutex> lk{ mutex };

            if (handles.size())
            {
                h = handles.back();
                handles.erase(--handles.end());
            }
        }

        if (h != Handle{})
        {
            pool.erase<T>(h);
            counter++;
        }
    }
}

template <typename T>
void reorderer(ObjectPool &pool, std::vector<Handle> &handles, std::mutex &mutex)
{
    for (auto i = 0; i < 10; i++)
    {
        std::this_thread::sleep_for(std::chrono::duration<double>(0.1));
        pool.defragment();
    }
}

SCENARIO("ObjectPools can be safely accessed from multiple threads.", "[objectpool][concurrent]")
{
    GIVEN("An empty ObjectPool")
    {
        using Object2 = std::array<char, OBJECT_SIZE_2>;
        using Object8 = std::array<char, OBJECT_SIZE_8>;
        using Object32 = std::array<char, OBJECT_SIZE_32>;

        const std::size_t runs{ 10000 };

        ObjectPool pool;

        std::vector<Handle> vecSize2;
        std::vector<Handle> vecSize8;
        std::vector<Handle> vecSize32;

        std::mutex vecSize2Mutex;
        std::mutex vecSize8Mutex;
        std::mutex vecSize32Mutex;

        std::atomic<unsigned> erasedSize2{};
        std::atomic<unsigned> erasedSize8{};
        std::atomic<unsigned> erasedSize32{};

        WHEN("There are 3 threads concurrently accessing the ObjectPool")
        {
            std::thread t1;
            std::thread t2;
            std::thread t3;

            THEN("If 2 threads are pushing and 1 thread is erasing")
            {
                t1 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t2 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t3 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };

                t1.join();
                t2.join();
                t3.join();

                REQUIRE(vecSize2.size() + erasedSize2 == runs * 2);
            }

            THEN("If 2 threads are erasing and 1 thread is pushing")
            {
                t1 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t2 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };
                t3 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };

                t1.join();
                t2.join();
                t3.join();

                REQUIRE(vecSize2.size() + erasedSize2 == runs);
            }

            THEN("If 1 thread is pushing, 1 thread is erasing and 1 thread is reordering")
            {
                t1 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t2 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };
                t3 = std::thread{ reorderer<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex) };

                t1.join();
                t2.join();
                t3.join();

                REQUIRE(vecSize2.size() + erasedSize2 == runs);
            }
        }

        WHEN("There are 9 threads concurrently accessing the ObjectPool")
        {
            std::thread t1;
            std::thread t2;
            std::thread t3;
            std::thread t4;
            std::thread t5;
            std::thread t6;
            std::thread t7;
            std::thread t8;
            std::thread t9;

            THEN("If 6 threads are pushing and 3 threads are erasing")
            {
                t1 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t2 = std::thread{ pusher<Object8, runs>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex) };
                t3 = std::thread{ pusher<Object32, runs>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex) };
                t4 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t5 = std::thread{ pusher<Object8, runs>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex) };
                t6 = std::thread{ pusher<Object32, runs>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex) };
                t7 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };
                t8 = std::thread{ eraser<Object8, runs>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(erasedSize8) };
                t9 = std::thread{ eraser<Object32, runs>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(erasedSize32) };

                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                t7.join();
                t8.join();
                t9.join();

                REQUIRE(vecSize2.size() + erasedSize2 == 2 * runs);
                REQUIRE(vecSize8.size() + erasedSize8 == 2 * runs);
                REQUIRE(vecSize32.size() + erasedSize32 == 2 * runs);
            }

            THEN("If 6 threads are erasing and 3 threads are pushing")
            {
                t1 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t2 = std::thread{ pusher<Object8, runs>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex) };
                t3 = std::thread{ pusher<Object32, runs>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex) };
                t4 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };
                t5 = std::thread{ eraser<Object8, runs>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(erasedSize8) };
                t6 = std::thread{ eraser<Object32, runs>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(erasedSize32) };
                t7 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };
                t8 = std::thread{ eraser<Object8, runs>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(erasedSize8) };
                t9 = std::thread{ eraser<Object32, runs>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(erasedSize32) };

                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                t7.join();
                t8.join();
                t9.join();

                REQUIRE(vecSize2.size() + erasedSize2 == runs);
                REQUIRE(vecSize8.size() + erasedSize8 == runs);
                REQUIRE(vecSize32.size() + erasedSize32 == runs);
            }

            THEN("If 3 threads are pushing, 3 threads are erasing and 3 threads are reordering")
            {
                t1 = std::thread{ pusher<Object2, runs>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t2 = std::thread{ pusher<Object8, runs>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex) };
                t3 = std::thread{ pusher<Object32, runs>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex) };
                t4 = std::thread{ eraser<Object2, runs>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(erasedSize2) };
                t5 = std::thread{ eraser<Object8, runs>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(erasedSize8) };
                t6 = std::thread{ eraser<Object32, runs>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(erasedSize32) };
                t7 = std::thread{ reorderer<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex) };
                t8 = std::thread{ reorderer<Object8>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex) };
                t9 = std::thread{ reorderer<Object32>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex) };

                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                t7.join();
                t8.join();
                t9.join();

                REQUIRE(vecSize2.size() + erasedSize2 == runs);
                REQUIRE(vecSize8.size() + erasedSize8 == runs);
                REQUIRE(vecSize32.size() + erasedSize32 == runs);
            }
        }
    }
}