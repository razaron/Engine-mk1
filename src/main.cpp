#include <utility>
#include <list>
#include <array>

#include "Misc.hpp"

using Array = alignedArray<char, 32 * 8, 64>;

typedef typename std::pair<int*, std::list<Array*>> Pair;

template <class T>
void foo()
{
	typedef typename T::second_type::value_type PagePtr;
	typedef typename std::remove_pointer<typename T::second_type::value_type>::type Page;

	auto ptr = new Page();

	std::cout << ptr->size() << std::endl;
}

int main()
{
	foo<Pair>();

	return 1;
}
