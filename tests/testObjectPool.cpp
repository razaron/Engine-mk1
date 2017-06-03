#include "ObjectPool.hpp"

#include "catch.hpp"

using namespace razaron::objectpool;

SCENARIO("ObjectPools can add pages as necessary", "[objectpool]") {

	GIVEN("An empty ObjectPool") {
		ObjectPool p;

		REQUIRE(p.capacity() == 0);

		WHEN("A single 8-byte object is pushed on") {
			p.emplace<std::array<char, 8>>(std::array<char, 8>{"1234567"});

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 8);
			}
		}
		WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " 8-byte objects are pushed on") {

			for (auto i = 0; i < 32; i++) {
				p.emplace<std::array<char, 8>>(std::array<char, 8>{"1234567"});
			}

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 8 * 2);
			}
		}

		WHEN("A single 16-byte object is pushed on") {
			p.emplace<std::array<char, 16>>(std::array<char, 16>{"1234567"});

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 16);
			}
		}
		WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " 16-byte objects are pushed on") {

			for (auto i = 0; i < 32; i++) {
				p.emplace<std::array<char, 16>>(std::array<char, 16>{"1234567"});
			}

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 16 * 2);
			}
		}

		WHEN("A single 32-byte object is pushed on") {
			p.emplace<std::array<char, 32>>(std::array<char, 32>{"1234567"});

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 32);
			}
		}
		WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " 32-byte objects are pushed on") {

			for (auto i = 0; i < 32; i++) {
				p.emplace<std::array<char, 32>>(std::array<char, 32>{"1234567"});
			}

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 32 * 2);
			}
		}

		WHEN("A single 64-byte object is pushed on") {
			p.emplace<std::array<char, 64>>(std::array<char, 64>{"1234567"});

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 64);
			}
		}
		WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " 64-byte objects are pushed on") {

			for (auto i = 0; i < 32; i++) {
				p.emplace<std::array<char, 64>>(std::array<char, 64>{"1234567"});
			}

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 64 * 2);
			}
		}

		WHEN("A single 128-byte object is pushed on") {
			p.emplace<std::array<char, 128>>(std::array<char, 128>{"1234567"});

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 128);
			}
		}
		WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " 128-byte objects are pushed on") {

			for (auto i = 0; i < 32; i++) {
				p.emplace<std::array<char, 128>>(std::array<char, 128>{"1234567"});
			}

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 128 * 2);
			}
		}

		WHEN("A single 256-byte object is pushed on") {
			p.emplace<std::array<char, 256>>(std::array<char, 256>{"1234567"});

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 256);
			}
		}
		WHEN(std::to_string(OBJECT_POOL_PAGE_LENGTH) + " 256-byte objects are pushed on") {

			for (auto i = 0; i < 32; i++) {
				p.emplace<std::array<char, 256>>(std::array<char, 256>{"1234567"});
			}

			THEN("the capacity changes") {
				REQUIRE(p.capacity() == 32 * 256 * 2);
			}
		}
	}
}

SCENARIO("ObjectPools can retrieve objects from Handles", "[objectpool]")
{

	ObjectPool p;

	Handle h8;
	Handle h16;
	Handle h32;
	Handle h64;
	Handle h128;
	Handle h256;

	REQUIRE(p.capacity() == 0);

	GIVEN("An object pool with some with a SINGLE page of objects")
	{
		h8 = p.emplace<std::array<char, 8>>(std::array<char, 8>{"hello"});
		h16 = p.emplace<std::array<char, 16>>(std::array<char, 16>{"hello"});
		h32 = p.emplace<std::array<char, 32>>(std::array<char, 32>{"hello"});
		h64 = p.emplace<std::array<char, 64>>(std::array<char, 64>{"hello"});
		h128 = p.emplace<std::array<char, 128>>(std::array<char, 128>{"hello"});
		h256 = p.emplace<std::array<char, 256>>(std::array<char, 256>{"hello"});

		THEN("the handles can be converted to objects")
		{
			std::array<char, 8>* p8 = p.getObject<std::array<char, 8>>(h8);
			std::array<char, 16>* p16 = p.getObject<std::array<char, 16>>(h16);
			std::array<char, 32>* p32 = p.getObject<std::array<char, 32>>(h32);
			std::array<char, 64>* p64 = p.getObject<std::array<char, 64>>(h64);
			std::array<char, 128>* p128 = p.getObject<std::array<char, 128>>(h128);
			std::array<char, 256>* p256 = p.getObject<std::array<char, 256>>(h256);

			REQUIRE((*p8)[4] == 'o');
			REQUIRE((*p16)[4] == 'o');
			REQUIRE((*p32)[4] == 'o');
			REQUIRE((*p64)[4] == 'o');
			REQUIRE((*p128)[4] == 'o');
			REQUIRE((*p256)[4] == 'o');
		}

	}

	GIVEN("An object pool with some with MULTIPLE pages of objects")
	{
		for (auto i = 0; i < 100; i++)
		{
			h8 = p.emplace<std::array<char, 8>>(std::array<char, 8>{"hello"});
			h16 = p.emplace<std::array<char, 16>>(std::array<char, 16>{"hello"});
			h32 = p.emplace<std::array<char, 32>>(std::array<char, 32>{"hello"});
			h64 = p.emplace<std::array<char, 64>>(std::array<char, 64>{"hello"});
			h128 = p.emplace<std::array<char, 128>>(std::array<char, 128>{"hello"});
			h256 = p.emplace<std::array<char, 256>>(std::array<char, 256>{"hello"});
		}

		THEN("the handles can be converted to objects")
		{
			std::array<char, 8>* p8 = p.getObject<std::array<char, 8>>(h8);
			std::array<char, 16>* p16 = p.getObject<std::array<char, 16>>(h16);
			std::array<char, 32>* p32 = p.getObject<std::array<char, 32>>(h32);
			std::array<char, 64>* p64 = p.getObject<std::array<char, 64>>(h64);
			std::array<char, 128>* p128 = p.getObject<std::array<char, 128>>(h128);
			std::array<char, 256>* p256 = p.getObject<std::array<char, 256>>(h256);

			REQUIRE((*p8)[4] == 'o');
			REQUIRE((*p16)[4] == 'o');
			REQUIRE((*p32)[4] == 'o');
			REQUIRE((*p64)[4] == 'o');
			REQUIRE((*p128)[4] == 'o');
			REQUIRE((*p256)[4] == 'o');
		}

	}
}
