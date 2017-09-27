#pragma once

#include <array>
#include <atomic>

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// For removing unused parameter warnings
#define UNUSED(x) (void)(x)

using expand_type = int[];

/* PATTERN is with the form:
	function(args)
	or
	(lambda)(args)
*/
#define VARIADIC_EXPANDER(PATTERN) 				\
    expand_type { 0, ((PATTERN), void(), 0)... }

// Returns a tuple of results. EXPRESSION is a brace enclosed function body
#define FOR_EACH_TUPLE(EXPRESSION, TUPLE) \
    std::apply([](auto... x) {            \
        return std::make_tuple(           \
            ([](auto element)      	      \
                EXPRESSION                \
    		)(x)...);          	     	  \
    }, TUPLE);

// EXPRESSION is a brace enclosed function body
#define FOR_EACH_TUPLE_ORDERED_VOID(EXPRESSION, TUPLE) 	\
    std::apply([](auto... x) {					 		\
        VARIADIC_EXPANDER( 								\
            ([](auto element)							\
                EXPRESSION 								\
            )(x)); 										\
    }, TUPLE);

/*! A unique unsigned int representing the next available unique ID. */
extern std::atomic<unsigned> g_nextID;

// Handling for pointers etc.
using HandleSize = std::size_t; /*!< Represents the size of Handle%d objects. */
using HandleIndex = unsigned short; /*!< Represents the indexed location of Handle%d objects. */
/*! Handles are used to abstract data access away from pointers. */
struct Handle
{
	HandleSize size{}; /*!< The size of the Handle%d object. */
	HandleIndex index{};/*!< The indexed location of the Handle%d object. */
	bool isFree{true};/*!< Whether the index denotes a free or occupied location. */

    /*! Basic equality comparator. */
    bool operator==(const Handle &rhs)
    {
        return (size == rhs.size && index == rhs.index && isFree == rhs.isFree);
    }
};

inline void* aligned_malloc(size_t size, size_t align) {
    void *result;
    #ifdef _WIN32
    result = _aligned_malloc(size, align);
    #else
    if(posix_memalign(&result, align, size)) result = 0;
    #endif
    return result;
}

inline void aligned_free(void *ptr) {
    #ifdef _WIN32
    _aligned_free(ptr);
    #else
    free(ptr);
    #endif

}

template <class T, std::size_t S, std::size_t A>
struct alignas(A) alignedArray
{
public:
	T* data() { return _array.data(); }
	std::size_t size() { return S; }
	std::size_t alignment() { return A; }

	T& operator [](std::size_t i) { return _array[i]; }
	void* operator new(std::size_t sz) { return aligned_malloc(sz, A); }
	void operator delete(void* ptr) { return aligned_free(ptr); }

private:
	std::array<T, S> _array{};
};
