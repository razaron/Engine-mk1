#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "TransformComponent.hpp"
#include "ObjectPool.hpp"

int main(int argc, char* argv[])
{
	std::clog.setstate(std::ios_base::failbit);

	std::cout << "sizeof(std::size_t) = " << sizeof(std::size_t) << std::endl;

	int result = Catch::Session().run(argc, argv);

	// global clean-up...

	return (result < 0xff ? result : 0xff);
}