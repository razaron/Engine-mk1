#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

int main(int argc, char *argv[])
{
    std::clog.setstate(std::ios_base::failbit);

    std::cout << "sizeof(std::size_t) = " << sizeof(std::size_t) << std::endl;

    int result = Catch::Session().run(argc, argv);

    std::cout << "Press enter to continue." << std::endl;
    std::cin.get();

    return (result < 0xff ? result : 0xff);
}
