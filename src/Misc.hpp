#pragma once

#include <array>
#include <type_traits>

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
		is_default_constructible = std::is_default_constructible<T>::value;
		is_copy_constructible = std::is_copy_constructible<T>::value;
		is_move_constructible = std::is_move_constructible<T>::value;
		//is_assignable = std::is_assignable<T>::value;
		is_copy_assignable = std::is_copy_assignable<T>::value;
		is_move_assignable = std::is_move_assignable<T>::value;
		//is_swappable_with = std::is_swappable_with<T>::value;
		//is_swappable = std::is_swappable<T>::value;
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
		//is_nothrow_swappable = std::is_nothrow_swappable<T>::value;
		is_nothrow_destructible = std::is_nothrow_destructible<T>::value;
		has_virtual_destructor = std::has_virtual_destructor<T>::value;
		//has_unique_object_representations = std::has_unique_object_representations<T>::value;
	}

	std::string typesStr()
	{
		std::string str{};

		if(is_void) str += "is_void ";
		if( is_null_pointer) str += "is_null_pointer ";
		if( is_integral) str += "is_integral ";
		if( is_floating_point) str += "is_floating_point ";
		if( is_array) str += "is_array ";
		if( is_pointer) str += "is_pointer ";
		if( is_lvalue_reference) str += "is_lvalue_reference ";
		if( is_rvalue_reference) str += "is_rvalue_reference ";
		if( is_member_object_pointer) str += "is_member_object_pointer ";
		if( is_member_function_pointer) str += "is_member_function_pointer ";
		if( is_enum) str += "is_enum ";
		if( is_union) str += "is_union ";
		if( is_class) str += "is_class ";
		if( is_function) str += "is_function ";

		// composite type categories:
		if( is_reference) str += "is_reference ";
		if( is_arithmetic) str += "is_arithmetic ";
		if( is_fundamental) str += "is_fundamental ";
		if( is_object) str += "is_object ";
		if( is_scalar) str += "is_scalar ";
		if( is_compound) str += "is_compound ";
		if( is_member_pointer) str += "is_member_pointer ";

		// type properties:
		if( is_const) str += "is_const ";
		if( is_volatile) str += "is_volatile ";
		if( is_trivial) str += "is_trivial ";
		if( is_trivially_copyable) str += "is_trivially_copyable ";
		if( is_standard_layout) str += "is_standard_layout ";
		if( is_pod) str += "is_pod ";
		if( is_empty) str += "is_empty ";
		if( is_polymorphic) str += "is_polymorphic ";
		if( is_abstract) str += "is_abstract ";
		if( is_final) str += "is_final ";
		//if( is_aggregate) str += " ";
		if( is_signed) str += "is_signed ";
		if( is_unsigned) str += "is_unsigned ";
		if( is_default_constructible) str += "is_default_constructible ";
		if( is_copy_constructible) str += "is_copy_constructible ";
		if( is_move_constructible) str += "is_move_constructible ";
		//if( is_assignable) str += " ";
		if( is_copy_assignable) str += "is_copy_assignable ";
		if( is_move_assignable) str += "is_move_assignable ";
		//if( is_swappable_with) str += " ";
		if( is_swappable) str += "is_swappable ";
		if( is_destructible) str += "is_destructible ";
		if( is_trivially_constructible) str += "is_trivially_constructible ";
		if( is_trivially_default_constructible) str += "is_trivially_default_constructible ";
		if( is_trivially_copy_constructible) str += "is_trivially_copy_constructible ";
		if( is_trivially_move_constructible) str += "is_trivially_move_constructible ";
		//if( is_trivially_assignable) str += " ";
		if( is_trivially_copy_assignable) str += "is_trivially_copy_assignable ";
		if( is_trivially_move_assignable) str += "is_trivially_move_assignable ";
		if( is_trivially_destructible) str += "is_trivially_destructible ";
		if( is_nothrow_constructible) str += "is_nothrow_constructible ";
		if( is_nothrow_default_constructible) str += "is_nothrow_default_constructible ";
		if( is_nothrow_copy_constructible) str += "is_nothrow_copy_constructible ";
		if( is_nothrow_move_constructible) str += "is_nothrow_move_constructible ";
		//if( is_nothrow_assignable) str += " ";
		if( is_nothrow_copy_assignable) str += "is_nothrow_copy_assignable ";
		if( is_nothrow_move_assignable) str += "is_nothrow_move_assignable ";
		//if( is_nothrow_swappable_with) str += " ";
		if( is_nothrow_swappable) str += "is_nothrow_swappable ";
		if( is_nothrow_destructible) str += "is_nothrow_destructible ";
		if( has_virtual_destructor) str += "has_virtual_destructor ";

		return str;
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
	T* data() { return m_array.data(); }
	std::size_t size() { return S; }
	std::size_t alignment() { return A; }

	T& operator [](std::size_t i) { return m_array[i]; }
	void* operator new(std::size_t sz) { return aligned_malloc(sz, A); }
	void operator delete(void* ptr) { return aligned_free(ptr); }

private:
	std::array<T, S> m_array{};
};
