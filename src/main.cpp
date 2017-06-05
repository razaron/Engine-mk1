#include <iostream>
#include <tuple>
#include <utility>

#include "ObjectPool.hpp"

using ArrayA = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2, OBJECT_POOL_PAGE_ALIGNMENT>;
using ArrayB = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4, OBJECT_POOL_PAGE_ALIGNMENT>;
using ArrayC = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8, OBJECT_POOL_PAGE_ALIGNMENT>;
using ArrayD = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16, OBJECT_POOL_PAGE_ALIGNMENT>;
using ArrayE = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32, OBJECT_POOL_PAGE_ALIGNMENT>;
using ArrayF = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_64, OBJECT_POOL_PAGE_ALIGNMENT>;

using PoolA = std::pair<Handle *, std::list<ArrayA *>>;
using PoolB = std::pair<Handle *, std::list<ArrayB *>>;
using PoolC = std::pair<Handle *, std::list<ArrayC *>>;
using PoolD = std::pair<Handle *, std::list<ArrayD *>>;
using PoolE = std::pair<Handle *, std::list<ArrayE *>>;
using PoolF = std::pair<Handle *, std::list<ArrayF *>>;

using PoolTuple = std::tuple<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>;

template <typename T>
void allocator(T v)
{
}

template <typename T>
void deleter(T v)
{
}

template <typename T>
void print(T v)
{
    std::cout << v << std::endl;
}

using expand_type = int[];

#define VARIADIC_EXPANDER(PATTERN) 				\
    expand_type { 0, ((PATTERN), void(), 0)... }

#define FOR_EACH_TUPLE(EXPRESSION, TUPLE) \
    std::apply([](auto... x) {            \
        return std::make_tuple(           \
            ([](auto v)           	      \
                EXPRESSION                \
    		)(x)...);          	     	  \
    }, TUPLE);

#define FOR_EACH_TUPLE_ORDERED_VOID(EXPRESSION, TUPLE) 	\
    std::apply([](auto... x) {					 		\
        VARIADIC_EXPANDER( 								\
            ([](auto v)  								\
                EXPRESSION 								\
            )(x)); 										\
    }, TUPLE);

int main()
{
    PoolTuple m_pools;
    std::array<int, 9> pair{1, 2,3,4,5,6,7,8,9};

    FOR_EACH_TUPLE_ORDERED_VOID({std::cout << v << std::endl;}, pair);

    auto doubled = FOR_EACH_TUPLE({return v*2;}, pair);

	FOR_EACH_TUPLE_ORDERED_VOID({std::cout << v << std::endl;}, doubled);

	std::cout << std::get<0>(doubled) << std::endl;

    return 1;
}
