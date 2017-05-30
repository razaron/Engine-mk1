#pragma once

#include <array>
#include <atomic>
#include <iostream>
#include <type_traits>

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

template <typename T>
struct TypeTraits {
	// primary type categories:
	bool is_void;
	bool is_null_pointer;
	bool is_integral;
	bool is_floating_point;
	bool is_array;
	bool is_pointer;
	bool is_lvalue_reference;
	bool is_rvalue_reference;
	bool is_member_object_pointer;
	bool is_member_function_pointer;
	bool is_enum;
	bool is_union;
	bool is_class;
	bool is_function;

	// composite type categories:
	bool is_reference;
	bool is_arithmetic;
	bool is_fundamental;
	bool is_object;
	bool is_scalar;
	bool is_compound;
	bool is_member_pointer;

	// type properties:
	bool is_const;
	bool is_volatile;
	bool is_trivial;
	bool is_trivially_copyable;
	bool is_standard_layout;
	bool is_pod;
	bool is_empty;
	bool is_polymorphic;
	bool is_abstract;
	bool is_final;
	//bool is_aggregate;
	bool is_signed;
	bool is_unsigned;
	bool is_constructible;
	bool is_default_constructible;
	bool is_copy_constructible;
	bool is_move_constructible;
	//bool is_assignable;
	bool is_copy_assignable;
	bool is_move_assignable;
	//bool is_swappable_with;
	bool is_swappable;
	bool is_destructible;
	bool is_trivially_constructible;
	bool is_trivially_default_constructible;
	bool is_trivially_copy_constructible;
	bool is_trivially_move_constructible;
	//bool is_trivially_assignable;
	bool is_trivially_copy_assignable;
	bool is_trivially_move_assignable;
	bool is_trivially_destructible;
	bool is_nothrow_constructible;
	bool is_nothrow_default_constructible;
	bool is_nothrow_copy_constructible;
	bool is_nothrow_move_constructible;
	//bool is_nothrow_assignable;
	bool is_nothrow_copy_assignable;
	bool is_nothrow_move_assignable;
	//bool is_nothrow_swappable_with;
	bool is_nothrow_swappable;
	bool is_nothrow_destructible;
	bool has_virtual_destructor;
	//bool has_unique_object_representations;

	TypeTraits<T>() {
		// primary type categories:
		is_void = std::is_void<T>::value;
		is_null_pointer = std::is_null_pointer<T>::value;
		is_integral = std::is_integral<T>::value;
		is_floating_point = std::is_floating_point<T>::value;
		is_array = std::is_array<T>::value;
		is_pointer = std::is_pointer<T>::value;
		is_lvalue_reference = std::is_lvalue_reference<T>::value;
		is_rvalue_reference = std::is_rvalue_reference<T>::value;
		is_member_object_pointer = std::is_member_object_pointer<T>::value;
		is_member_function_pointer = std::is_member_function_pointer<T>::value;
		is_enum = std::is_enum<T>::value;
		is_union = std::is_union<T>::value;
		is_class = std::is_class<T>::value;
		is_function = std::is_function<T>::value;

		// composite type categories:
		is_reference = std::is_reference<T>::value;
		is_arithmetic = std::is_arithmetic<T>::value;
		is_fundamental = std::is_fundamental<T>::value;
		is_object = std::is_object<T>::value;
		is_scalar = std::is_scalar<T>::value;
		is_compound = std::is_compound<T>::value;
		is_member_pointer = std::is_member_pointer<T>::value;

		// type properties:
		is_const = std::is_const<T>::value;
		is_volatile = std::is_volatile<T>::value;
		is_trivial = std::is_trivial<T>::value;
		is_trivially_copyable = std::is_trivially_copyable<T>::value;
		is_standard_layout = std::is_standard_layout<T>::value;
		is_pod = std::is_pod<T>::value;
		is_empty = std::is_empty<T>::value;
		is_polymorphic = std::is_polymorphic<T>::value;
		is_abstract = std::is_abstract<T>::value;
		is_final = std::is_final<T>::value;
		//is_aggregate = std::is_aggregate<T>::value;
		is_signed = std::is_signed<T>::value;
		is_unsigned = std::is_unsigned<T>::value;
		is_constructible = std::is_constructible<T>::value;
		is_default_constructible = std::is_default_constructible<T>::value;
		is_copy_constructible = std::is_copy_constructible<T>::value;
		is_move_constructible = std::is_move_constructible<T>::value;
		//is_assignable = std::is_assignable<T>::value;
		is_copy_assignable = std::is_copy_assignable<T>::value;
		is_move_assignable = std::is_move_assignable<T>::value;
		//is_swappable_with = std::is_swappable_with<T>::value;
		is_swappable = std::is_swappable<T>::value;
		is_destructible = std::is_destructible<T>::value;
		is_trivially_constructible = std::is_trivially_constructible<T>::value;
		is_trivially_default_constructible = std::is_trivially_default_constructible<T>::value;
		is_trivially_copy_constructible = std::is_trivially_copy_constructible<T>::value;
		is_trivially_move_constructible = std::is_trivially_move_constructible<T>::value;
		//is_trivially_assignable = std::is_trivially_assignable<T>::value;
		is_trivially_copy_assignable = std::is_trivially_copy_assignable<T>::value;
		is_trivially_move_assignable = std::is_trivially_move_assignable<T>::value;
		is_trivially_destructible = std::is_trivially_destructible<T>::value;
		is_nothrow_constructible = std::is_nothrow_constructible<T>::value;
		is_nothrow_default_constructible = std::is_nothrow_default_constructible<T>::value;
		is_nothrow_copy_constructible = std::is_nothrow_copy_constructible<T>::value;
		is_nothrow_move_constructible = std::is_nothrow_move_constructible<T>::value;
		//is_nothrow_assignable = std::is_nothrow_assignable<T>::value;
		is_nothrow_copy_assignable = std::is_nothrow_copy_assignable<T>::value;
		is_nothrow_move_assignable = std::is_nothrow_move_assignable<T>::value;
		//is_nothrow_swappable_with = std::is_nothrow_swappable_with<T>::value;
		is_nothrow_swappable = std::is_nothrow_swappable<T>::value;
		is_nothrow_destructible = std::is_nothrow_destructible<T>::value;
		has_virtual_destructor = std::has_virtual_destructor<T>::value;
		//has_unique_object_representations = std::has_unique_object_representations<T>::value;
	}
};

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