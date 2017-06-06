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
            p.emplace<std::array<char, OBJECT_SIZE_2>>(std::array<char, OBJECT_SIZE_2>{"1234567"});

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_2-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_2>>(std::array<char, OBJECT_SIZE_2>{"1234567"});
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_4-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_4>>(std::array<char, OBJECT_SIZE_4>{"1234567"});

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_4-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_4>>(std::array<char, OBJECT_SIZE_4>{"1234567"});
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_8-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_8>>(std::array<char, OBJECT_SIZE_8>{"1234567"});

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_8-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_8>>(std::array<char, OBJECT_SIZE_8>{"1234567"});
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_16-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_16>>(std::array<char, OBJECT_SIZE_16>{"1234567"});

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_16-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_16>>(std::array<char, OBJECT_SIZE_16>{"1234567"});
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_32-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_32>>(std::array<char, OBJECT_SIZE_32>{"1234567"});

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_32-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_32>>(std::array<char, OBJECT_SIZE_32>{"1234567"});
            }

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32 * 2);
            }
        }

        WHEN("A single OBJECT_SIZE_64-byte object is pushed on")
        {
            p.emplace<std::array<char, OBJECT_SIZE_64>>(std::array<char, OBJECT_SIZE_64>{"1234567"});

            THEN("the capacity changes")
            {
                REQUIRE(p.capacity() == OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_64);
            }
        }
        WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " OBJECT_SIZE_64-byte objects are pushed on")
        {

            for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
            {
                p.emplace<std::array<char, OBJECT_SIZE_64>>(std::array<char, OBJECT_SIZE_64>{"1234567"});
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
        h2 = p.emplace<std::array<char, OBJECT_SIZE_2>>(std::array<char, OBJECT_SIZE_2>{"first"});
        h4 = p.emplace<std::array<char, OBJECT_SIZE_4>>(std::array<char, OBJECT_SIZE_4>{"second"});
        h8 = p.emplace<std::array<char, OBJECT_SIZE_8>>(std::array<char, OBJECT_SIZE_8>{"third"});
        h16 = p.emplace<std::array<char, OBJECT_SIZE_16>>(std::array<char, OBJECT_SIZE_16>{"fourth"});
        h32 = p.emplace<std::array<char, OBJECT_SIZE_32>>(std::array<char, OBJECT_SIZE_32>{"fifth"});
        h64 = p.emplace<std::array<char, OBJECT_SIZE_64>>(std::array<char, OBJECT_SIZE_64>{"sixth"});

        THEN("the handles can be converted to objects")
        {
            auto p2 = *p.getObject<std::array<char, OBJECT_SIZE_2>>(h2);
            auto p4 = *p.getObject<std::array<char, OBJECT_SIZE_4>>(h4);
            auto p8 = *p.getObject<std::array<char, OBJECT_SIZE_8>>(h8);
            auto p16 = *p.getObject<std::array<char, OBJECT_SIZE_16>>(h16);
            auto p32 = *p.getObject<std::array<char, OBJECT_SIZE_32>>(h32);
            auto p64 = *p.getObject<std::array<char, OBJECT_SIZE_64>>(h64);

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
            h2 = p.emplace<std::array<int, OBJECT_SIZE_2/sizeof(int)>>(std::array<int, OBJECT_SIZE_2/sizeof(int)>{2});
            h4 = p.emplace<std::array<int, OBJECT_SIZE_4/sizeof(int)>>(std::array<int, OBJECT_SIZE_4/sizeof(int)>{4});
            h8 = p.emplace<std::array<int, OBJECT_SIZE_8/sizeof(int)>>(std::array<int, OBJECT_SIZE_8/sizeof(int)>{8});
            h16 = p.emplace<std::array<int, OBJECT_SIZE_16/sizeof(int)>>(std::array<int, OBJECT_SIZE_16/sizeof(int)>{16});
            h32 = p.emplace<std::array<int, OBJECT_SIZE_32/sizeof(int)>>(std::array<int, OBJECT_SIZE_32/sizeof(int)>{32});
            h64 = p.emplace<std::array<int, OBJECT_SIZE_64/sizeof(int)>>(std::array<int, OBJECT_SIZE_64/sizeof(int)>{64});
        }

        THEN("the handles can be converted to objects")
        {
            for (HandleIndex i = 0; i < 100; i++)
            {

                auto p2 = *p.getObject<std::array<int, OBJECT_SIZE_2 / sizeof(int)>>({OBJECT_SIZE_2, i, false});
                auto p4 = *p.getObject<std::array<int, OBJECT_SIZE_4 / sizeof(int)>>({OBJECT_SIZE_4, i, false});
                auto p8 = *p.getObject<std::array<int, OBJECT_SIZE_8 / sizeof(int)>>({OBJECT_SIZE_8, i, false});
                auto p16 = *p.getObject<std::array<int, OBJECT_SIZE_16 / sizeof(int)>>({OBJECT_SIZE_16, i, false});
                auto p32 = *p.getObject<std::array<int, OBJECT_SIZE_32 / sizeof(int)>>({OBJECT_SIZE_32, i, false});
                auto p64 = *p.getObject<std::array<int, OBJECT_SIZE_64 / sizeof(int)>>({OBJECT_SIZE_64, i, false});

                REQUIRE(p2[0] == 2);
                REQUIRE(p2[1] == 0);
                REQUIRE(p4[0] == 4);
                REQUIRE(p4[1] == 0);
                REQUIRE(p8[0] == 8);
                REQUIRE(p8[1] == 0);
                REQUIRE(p16[0] == 16);
                REQUIRE(p16[1] == 0);
                REQUIRE(p32[0] == 32);
                REQUIRE(p32[1] == 0);
                REQUIRE(p64[0] == 64);
                REQUIRE(p64[1] == 0);
            }
        }
    }
}

typedef std::array<char, OBJECT_SIZE_2> Arr;

SCENARIO("You removes objects from anywhere in the ObjectPool")
{

    GIVEN("The ObjectPool {filled, filled, filled, empty...}")
    {
        ObjectPool pool;

        REQUIRE(pool.capacity() == 0);

        // Pushed into OBJECT_SIZE_2-byte pool
        auto first = pool.emplace<Arr>('f', 'i', 'r', 's', 't', '\0');
        auto p1 = pool.getObject<Arr>(first);

        // Pushed into OBJECT_SIZE_2-byte pool
        auto second = pool.emplace<Arr>('s', 'e', 'c', 'o', 'n', 'd', '\0');
        auto p2 = pool.getObject<Arr>(second);

        // Pushed into OBJECT_SIZE_2-byte pool
        auto third = pool.emplace<Arr>('t', 'h', 'i', 'r', 'd', '\0');
        auto p3 = pool.getObject<Arr>(third);

        REQUIRE(std::string{p1->data()} == std::string{"first"});
        REQUIRE(std::string{p2->data()} == std::string{"second"});
        REQUIRE(std::string{p3->data()} == std::string{"third"});

        WHEN("Removing an object from position 0")
        {
            pool.removeObject<Arr>(first);

            auto ptr = reinterpret_cast<Handle *>(p1);

            // The pointer for first has become a free pointer at position 0 pointing to position 3
            REQUIRE(ptr->isFree == true);
            REQUIRE(ptr->size == OBJECT_SIZE_2);
            REQUIRE(ptr->index == 3);

            THEN("Removing an object from position 2")
            {
                pool.removeObject<Arr>(third);

                // Re-use p3 to see what the position looks like now
                auto ptr = reinterpret_cast<Handle *>(p3);

                // The pointer for third has become a free pointer at position 2 pointing to position 3
                REQUIRE(ptr->isFree == true);
                REQUIRE(ptr->size == OBJECT_SIZE_2);
                REQUIRE(ptr->index == 3);

                // Calling getObject again fails, returning a nullptr
                auto nullPointer = pool.getObject<Arr>(third);
                REQUIRE(nullPointer == nullptr);

                pool.emplace<Arr>('a', 'l', 'p', 'h', 'a', '\0');
                pool.emplace<Arr>('g', 'a', 'm', 'm', 'a', '\0');

                REQUIRE(std::string{p1->data()} == std::string{"alpha"});
                REQUIRE(std::string{p2->data()} == std::string{"second"});
                REQUIRE(std::string{p3->data()} == std::string{"gamma"});
            }
        }
    }
}
