#include <atomic>
#include <thread>

#include <catch.hpp>

#include "ObjectPool.hpp"

using namespace razaron::objectpool;

SCENARIO("ObjectPools can add pages as necessary", "[objectpool]")
{
    GIVEN("An empty ObjectPool")
    {
        ObjectPool p;

        REQUIRE(p.capacity() == 0);

        WHEN("A single OBJECT_SIZE_2-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_2>>(std::array<char, OBJECT_SIZE_2>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_2-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_2>>(std::array<char, OBJECT_SIZE_2>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_4-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_4>>(std::array<char, OBJECT_SIZE_4>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_4-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_4>>(std::array<char, OBJECT_SIZE_4>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_8-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_8>>(std::array<char, OBJECT_SIZE_8>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_8-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_8>>(std::array<char, OBJECT_SIZE_8>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_16-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_16>>(std::array<char, OBJECT_SIZE_16>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_16-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_16>>(std::array<char, OBJECT_SIZE_16>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_32-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_32>>(std::array<char, OBJECT_SIZE_32>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_32-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_32>>(std::array<char, OBJECT_SIZE_32>{ "1234567" });
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_64-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_64>>(std::array<char, OBJECT_SIZE_64>{ "1234567" });

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_64);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_64-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_64>>(std::array<char, OBJECT_SIZE_64>{ "1234567" });
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

    Handle h2;
    Handle h4;
    Handle h8;
    Handle h16;
    Handle h32;
    Handle h64;

    REQUIRE(p.capacity() == 0);

    GIVEN("An object pool with a SINGLE page of objects")
    {
        h2 = p.emplace<std::array<char, OBJECT_SIZE_2>>(std::array<char, OBJECT_SIZE_2>{ "first" });
        h4 = p.emplace<std::array<char, OBJECT_SIZE_4>>(std::array<char, OBJECT_SIZE_4>{ "second" });
        h8 = p.emplace<std::array<char, OBJECT_SIZE_8>>(std::array<char, OBJECT_SIZE_8>{ "third" });
        h16 = p.emplace<std::array<char, OBJECT_SIZE_16>>(std::array<char, OBJECT_SIZE_16>{ "fourth" });
        h32 = p.emplace<std::array<char, OBJECT_SIZE_32>>(std::array<char, OBJECT_SIZE_32>{ "fifth" });
        h64 = p.emplace<std::array<char, OBJECT_SIZE_64>>(std::array<char, OBJECT_SIZE_64>{ "sixth" });

        THEN("the handles can be converted to objects")
        {
            auto p2 = *p.get<std::array<char, OBJECT_SIZE_2>>(h2);
            auto p4 = *p.get<std::array<char, OBJECT_SIZE_4>>(h4);
            auto p8 = *p.get<std::array<char, OBJECT_SIZE_8>>(h8);
            auto p16 = *p.get<std::array<char, OBJECT_SIZE_16>>(h16);
            auto p32 = *p.get<std::array<char, OBJECT_SIZE_32>>(h32);
            auto p64 = *p.get<std::array<char, OBJECT_SIZE_64>>(h64);

            REQUIRE(p2[4] == 't');
            REQUIRE(p4[4] == 'n');
            REQUIRE(p8[4] == 'd');
            REQUIRE(p16[4] == 't');
            REQUIRE(p32[4] == 'h');
            REQUIRE(p64[2] == 'x');
        }
    }

    GIVEN("An object pool with some with MULTIPLE pages of objects")
    {
        for (auto i = 0; i < 100; i++)
        {
            h2 = p.emplace<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>(std::array<int, OBJECT_SIZE_2 / sizeof(int)>{ 2 });
            h4 = p.emplace<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>(std::array<int, OBJECT_SIZE_4 / sizeof(int)>{ 4 });
            h8 = p.emplace<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>(std::array<int, OBJECT_SIZE_8 / sizeof(int)>{ 8 });
            h16 = p.emplace<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>(std::array<int, OBJECT_SIZE_16 / sizeof(int)>{ 16 });
            h32 = p.emplace<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>(std::array<int, OBJECT_SIZE_32 / sizeof(int)>{ 32 });
            h64 = p.emplace<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>(std::array<int, OBJECT_SIZE_64 / sizeof(int)>{ 64 });
        }

        THEN("the handles can be converted to objects")
        {
            bool result = true;

            for (HandleIndex i = 0; i < 100; i++)
            {

                auto p2 = *p.get<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>({ OBJECT_SIZE_2, i, false });
                auto p4 = *p.get<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>({ OBJECT_SIZE_4, i, false });
                auto p8 = *p.get<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>({ OBJECT_SIZE_8, i, false });
                auto p16 = *p.get<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>({ OBJECT_SIZE_16, i, false });
                auto p32 = *p.get<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>({ OBJECT_SIZE_32, i, false });
                auto p64 = *p.get<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>({ OBJECT_SIZE_64, i, false });


                if(p2[0] != 2)
                    result = false;
                if(p2[1] != 0)
                    result = false;
                if(p4[0] != 4)
                    result = false;
                if(p4[1] != 0)
                    result = false;
                if(p8[0] != 8)
                    result = false;
                if(p8[1] != 0)
                    result = false;
                if(p16[0] != 16)
                    result = false;
                if(p16[1] != 0)
                    result = false;
                if(p32[0] != 32)
                    result = false;
                if(p32[1] != 0)
                    result = false;
                if(p64[0] != 64)
                    result = false;
                if(p64[1] != 0)
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

            auto ptr = reinterpret_cast<Handle *>(p1);

            // The pointer for first has become a free pointer at position 0 pointing to position 3
            REQUIRE(ptr->isFree == true);
            REQUIRE(ptr->size == OBJECT_SIZE_2);
            REQUIRE(ptr->index == 3);

            THEN("Removing an object from position 2")
            {
                pool.erase<Arr>(third);

                // Re-use p3 to see what the position looks like now
                auto ptr = reinterpret_cast<Handle *>(p3);

                // The pointer for third has become a free pointer at position 2 pointing to position 3
                REQUIRE(ptr->isFree == true);
                REQUIRE(ptr->size == OBJECT_SIZE_2);
                REQUIRE(ptr->index == 3);

                // Calling get again fails, returning a nullptr
                auto nullPointer = pool.get<Arr>(third);
                REQUIRE(nullPointer == nullptr);

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

        // Creates 4 pages in every sub-pool
        for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH*4; i++)
        {
            p.emplace<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>(std::array<int, OBJECT_SIZE_2 / sizeof(int)>{ 2 });
            p.emplace<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>(std::array<int, OBJECT_SIZE_4 / sizeof(int)>{ 4 });
            p.emplace<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>(std::array<int, OBJECT_SIZE_8 / sizeof(int)>{ 8 });
            p.emplace<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>(std::array<int, OBJECT_SIZE_16 / sizeof(int)>{ 16 });
            p.emplace<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>(std::array<int, OBJECT_SIZE_32 / sizeof(int)>{ 32 });
            p.emplace<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>(std::array<int, OBJECT_SIZE_64 / sizeof(int)>{ 64 });
        }

        std::size_t expected = 5*OBJECT_POOL_PAGE_LENGTH*(OBJECT_SIZE_2+OBJECT_SIZE_4+OBJECT_SIZE_8+OBJECT_SIZE_16+OBJECT_SIZE_32+OBJECT_SIZE_64);

        REQUIRE(expected == p.capacity());

        // Erases the first 2 pages from every sub-pool
        for (HandleIndex i = 0; i < OBJECT_POOL_PAGE_LENGTH*2; i++)
        {

            p.erase<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>({ OBJECT_SIZE_2, i, false });
            p.erase<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>({ OBJECT_SIZE_4, i, false });
            p.erase<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>({ OBJECT_SIZE_8, i, false });
            p.erase<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>({ OBJECT_SIZE_16, i, false });
            p.erase<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>({ OBJECT_SIZE_32, i, false });
            p.erase<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>({ OBJECT_SIZE_64, i, false });
        }

        WHEN("The 2nd 2 pages have their objects reordered.")
        {
            // Reorders everything in the 2nd 2 pages of every sub-pool
            for (HandleIndex i = OBJECT_POOL_PAGE_LENGTH*2; i < OBJECT_POOL_PAGE_LENGTH*4; i++)
            {

                p.reorder<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>({ OBJECT_SIZE_2, i, false });
                p.reorder<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>({ OBJECT_SIZE_4, i, false });
                p.reorder<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>({ OBJECT_SIZE_8, i, false });
                p.reorder<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>({ OBJECT_SIZE_16, i, false });
                p.reorder<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>({ OBJECT_SIZE_32, i, false });
                p.reorder<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>({ OBJECT_SIZE_64, i, false });
            }

            // Verifies that the 2nd 2 pages have been reordered
            bool result = true;
            for (HandleIndex i = 0; i < OBJECT_POOL_PAGE_LENGTH*2; i++)
            {

                auto p2 = *p.get<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>({ OBJECT_SIZE_2, i, false });
                auto p4 = *p.get<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>({ OBJECT_SIZE_4, i, false });
                auto p8 = *p.get<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>({ OBJECT_SIZE_8, i, false });
                auto p16 = *p.get<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>({ OBJECT_SIZE_16, i, false });
                auto p32 = *p.get<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>({ OBJECT_SIZE_32, i, false });
                auto p64 = *p.get<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>({ OBJECT_SIZE_64, i, false });


                if(p2[0] != 2)
                    result = false;
                if(p2[1] != 0)
                    result = false;
                if(p4[0] != 4)
                    result = false;
                if(p4[1] != 0)
                    result = false;
                if(p8[0] != 8)
                    result = false;
                if(p8[1] != 0)
                    result = false;
                if(p16[0] != 16)
                    result = false;
                if(p16[1] != 0)
                    result = false;
                if(p32[0] != 32)
                    result = false;
                if(p32[1] != 0)
                    result = false;
                if(p64[0] != 64)
                    result = false;
                if(p64[1] != 0)
                    result = false;
            }

            REQUIRE(result == true);

            THEN("Shrinking the ObjectPool will release 3 pages worth of memory")
            {
                p.shrink();

                std::size_t expected = 2*OBJECT_POOL_PAGE_LENGTH*(OBJECT_SIZE_2+OBJECT_SIZE_4+OBJECT_SIZE_8+OBJECT_SIZE_16+OBJECT_SIZE_32+OBJECT_SIZE_64);

                REQUIRE(expected == p.capacity());
            }
        }
    }
}

template <typename T>
void emplacer(ObjectPool &pool, T value, std::vector<Handle> &handles, std::mutex &mutex)
{
    for (auto i = 0; i < 10000; i++)
    {
        Handle h{};
        if (i % 2)
            h = pool.emplace<T>(value);
        else
            h = pool.push<T>(value);

        {
            std::lock_guard<std::mutex> lk{ mutex };
            handles.push_back(std::move(h));
        }
    }
}

template <typename T>
void eraser(ObjectPool &pool, std::vector<Handle> &handles, std::mutex &mutex, std::atomic<unsigned> &counter)
{
    for (auto i = 0; i < 10000; i++)
    {
        Handle h{};

        {
            std::lock_guard<std::mutex> lk{mutex};

            if(handles.size())
            {
                h = handles.back();
                handles.erase(--handles.end());
            }
        }

        if(h.size)
        {
            pool.erase<T>(h);
            counter++;
        }
    }
}

template <typename T>
void reorderer(ObjectPool &pool, std::vector<Handle> &handles, std::mutex &mutex)
{
    for (auto i = 0; i < 10000; i++)
    {
        Handle h{};

        {
            std::lock_guard<std::mutex> lk{mutex};

            if(handles.size())
                h = handles.back();
        }

        if(h.size)
            pool.get<T>(h);
    }
}

SCENARIO("ObjectPools can be safely accessed from multiple threads.", "[objectpool][concurrent]")
{
    GIVEN("An empty ObjectPool")
    {
        using Object2 = std::array<char, OBJECT_SIZE_2>;
        using Object8 = std::array<char, OBJECT_SIZE_8>;
        using Object32 = std::array<char, OBJECT_SIZE_32>;

        ObjectPool pool;

        std::vector<Handle> vecSize2;
        std::vector<Handle> vecSize8;
        std::vector<Handle> vecSize32;


        std::mutex vecSize2Mutex;
        std::mutex vecSize8Mutex;
        std::mutex vecSize32Mutex;

        std::atomic<unsigned> counterSize2{};
        std::atomic<unsigned> counterSize8{};
        std::atomic<unsigned> counterSize32{};

        WHEN("There are 3 threads concurrently accessing the ObjectPool")
        {
            std::thread t1;
            std::thread t2;
            std::thread t3;

            THEN("If 2 threads are emplacing and 1 thread is erasing")
            {
                t1 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t2 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t3 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};

                t1.join();
                t2.join();
                t3.join();

                REQUIRE(vecSize2.size() + counterSize2 == 20000);
            }

            THEN("If 2 threads are erasing and 1 thread is emplacing")
            {
                t1 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t2 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};
                t3 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};

                t1.join();
                t2.join();
                t3.join();

                REQUIRE(vecSize2.size() + counterSize2 == 10000);
            }

            THEN("If 1 thread is emplacing, 1 thread is erasing and 1 thread is reordering")
            {
                t1 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t2 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};
                t3 = std::thread{reorderer<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex)};

                t1.join();
                t2.join();
                t3.join();

                REQUIRE(vecSize2.size() + counterSize2 == 10000);
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

            THEN("If 6 threads are emplacing and 3 threads are erasing")
            {
                t1 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t2 = std::thread{emplacer<Object8>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex)};
                t3 = std::thread{emplacer<Object32>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex)};
                t4 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t5 = std::thread{emplacer<Object8>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex)};
                t6 = std::thread{emplacer<Object32>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex)};
                t7 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};
                t8 = std::thread{eraser<Object8>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(counterSize8)};
                t9 = std::thread{eraser<Object32>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(counterSize32)};

                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                t7.join();
                t8.join();
                t9.join();

                REQUIRE(vecSize2.size() + counterSize2 == 20000);
                REQUIRE(vecSize8.size() + counterSize8 == 20000);
                REQUIRE(vecSize32.size() + counterSize32 == 20000);
            }

            THEN("If 6 threads are erasing and 3 threads are emplacing")
            {
                t1 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t2 = std::thread{emplacer<Object8>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex)};
                t3 = std::thread{emplacer<Object32>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex)};
                t4 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};
                t5 = std::thread{eraser<Object8>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(counterSize8)};
                t6 = std::thread{eraser<Object32>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(counterSize32)};
                t7 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};
                t8 = std::thread{eraser<Object8>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(counterSize8)};
                t9 = std::thread{eraser<Object32>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(counterSize32)};

                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                t7.join();
                t8.join();
                t9.join();

                REQUIRE(vecSize2.size() + counterSize2 == 10000);
                REQUIRE(vecSize8.size() + counterSize8 == 10000);
                REQUIRE(vecSize32.size() + counterSize32 == 10000);
            }

            THEN("If 3 threads are emplacing, 3 threads are erasing and 3 threads are reordering")
            {
                t1 = std::thread{emplacer<Object2>, std::ref(pool), Object2{}, std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t2 = std::thread{emplacer<Object8>, std::ref(pool), Object8{}, std::ref(vecSize8), std::ref(vecSize8Mutex)};
                t3 = std::thread{emplacer<Object32>, std::ref(pool), Object32{}, std::ref(vecSize32), std::ref(vecSize32Mutex)};
                t4 = std::thread{eraser<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex), std::ref(counterSize2)};
                t5 = std::thread{eraser<Object8>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex), std::ref(counterSize8)};
                t6 = std::thread{eraser<Object32>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex), std::ref(counterSize32)};
                t7 = std::thread{reorderer<Object2>, std::ref(pool), std::ref(vecSize2), std::ref(vecSize2Mutex)};
                t8 = std::thread{reorderer<Object8>, std::ref(pool), std::ref(vecSize8), std::ref(vecSize8Mutex)};
                t9 = std::thread{reorderer<Object32>, std::ref(pool), std::ref(vecSize32), std::ref(vecSize32Mutex)};

                t1.join();
                t2.join();
                t3.join();
                t4.join();
                t5.join();
                t6.join();
                t7.join();
                t8.join();
                t9.join();

                REQUIRE(vecSize2.size() + counterSize2 == 10000);
                REQUIRE(vecSize8.size() + counterSize8 == 10000);
                REQUIRE(vecSize32.size() + counterSize32 == 10000);
            }
        }
    }
}
