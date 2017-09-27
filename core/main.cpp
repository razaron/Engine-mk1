#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>

#include "EventStream.hpp"

template <typename T, std::size_t... Ind>
void printTuple(T tuple, std::index_sequence<Ind...>)
{
    // ((expression), ...);
    ((std::cout << std::get<Ind>(tuple) << " "), ...);
    std::cout << std::endl;
}

template <typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
decltype(auto) print(const T &t)
{
    return printTuple(t, Indices{});
}

class Test
{
  public:
    template <class... Args>
    Test(Args... p_args);
    Test(int i);
};

template <class... Args>
Test::Test(Args... p_args)
{
    auto t = std::make_tuple(p_args...);

    print<decltype(t), std::tuple_size<decltype(t)>::value>(t);
}

Test::Test(int i)
{
    std::cout << "ping " << i << std::endl;
}

int main()
{
    //Test a{1, "a", "lol"};
    //Test b{1};

    std::map<unsigned, std::pair<float, double>> map;
    map[1u] = std::make_pair(1.f, 1.0);
    map[2u] = std::make_pair(2.f, 2.0);
    map[3u] = std::make_pair(3.f, 3.0);
    map[4u] = std::make_pair(4.f, 4.0);

    using namespace razaron::eventstream;
    EventStream e{};
    e.registerHandler(EventType::EVENT_1, [](Event &e) { std::cout << "Hello"; });
    e.extendHandler(EventType::EVENT_1, [](Event &e) { std::cout << " World!!!!!\n"; });

    e.pushEvent(Event{ 0u, EventType::EVENT_1, nullptr }, StreamType::INCOMING);
    e.processEvents();

    return 0;
}
