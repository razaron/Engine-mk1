#pragma once

#include <array>
#include <atomic>
#include <iostream>

struct Handle
{
	size_t size;
	unsigned short index;
	bool free;
};

template<typename T> 
struct AtomicCounter {
	std::atomic<T> value{0};

	T operator ++(int) {
		return value++;
	}

	T operator --(int) {
		return value--;
	}

	T operator =(T desired) {
		return value = desired;
	}

	T get() {
		return value.load();
	}
};

extern AtomicCounter<unsigned> g_constructed;
extern AtomicCounter<unsigned> g_destroyed;

template <class T, std::size_t S, std::size_t A>
struct alignas(A) alignedArray
{
public:
	T* data() { return m_array.data(); }
	std::size_t size() { return S; }
	std::size_t alignment() { return A; }

	T& operator [](std::size_t i) { return m_array[i]; }
	void* operator new(std::size_t sz) { return aligned_alloc(sz, A); }
	void operator delete(void* ptr) { return aligned_free(ptr); }

private:
	std::array<T, S> m_array{};
};

inline void* aligned_alloc(size_t size, size_t align) {
	void* result = nullptr;
#ifdef _MSC_VER 
	result = _aligned_malloc(size, align);
#else 
	if (posix_memalign(&result, align, size)) result = 0;
#endif
	return result;
}

inline void aligned_free(void *ptr) {
#ifdef _MSC_VER 
	_aligned_free(ptr);
#else 
	free(ptr);
#endif

}

// --- Reversed iterable
template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse(T&& iterable) { return{ iterable }; }