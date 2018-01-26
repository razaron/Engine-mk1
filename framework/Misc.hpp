#pragma once

#include <array>
#include <atomic>
#include <algorithm>
#include <bitset>
#include <random>

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

// Helper function
template <typename T>
bool includes(T a, T b)
{
	int rem = b.size();
	for (auto &c : a)
	{
		if (b.find(c) != b.end())
			rem--;
	}

	return !rem;
}

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

// UUID struct 64 bits
struct UUID64
{
	std::bitset<64> uuid;

	UUID64()
	{
		std::random_device r;
		std::seed_seq s{ r(), r(), r(), r(), r(), r(), r(), r() };
		std::mt19937_64 e{ s };

		auto random = e();
		uuid = std::bitset<64>{ random };
	}

	UUID64(int) : uuid(std::bitset<64>{}) {}

	template <std::size_t N>
	UUID64(std::bitset<N> mask)
	{
		std::random_device r;
		std::seed_seq s{ r(), r(), r(), r(), r(), r(), r(), r() };
		std::mt19937_64 e{ s };

		auto random = e();
		uuid = std::bitset<64>{ random };

		for (std::size_t i = 0; i < mask.size(); i++)
		{
			uuid[i] = mask[i];
		}
	}

	bool operator==(const UUID64 &rhs) const
	{
		return uuid == rhs.uuid;
	}

	bool operator!=(const UUID64 &rhs) const
	{
		return uuid != rhs.uuid;
	}
};

struct UUID64Cmp {
	bool operator()(const UUID64& lhs, const UUID64& rhs) const {
		return lhs.uuid.to_ullong() < rhs.uuid.to_ullong();
	}
};