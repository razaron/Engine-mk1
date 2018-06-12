#pragma once

#include <array>
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

// Helper function
template <typename T>
inline bool includes(T a, T b)
{
    int remaining = b.size();
    for (auto &c : a)
    {
        if (b.find(c) != b.end())
            remaining--;
    }

    return !remaining;
}

// Handling for pointers etc.
using HandleSize = std::size_t;  /*!< Represents the size of Handle%d objects. */
using HandleIndex = std::size_t; /*!< Represents the indexed location of Handle%d objects. */

/*! Handles are used to abstract data access away from pointers. */
struct Handle
{
    HandleSize size{};                /*!< The size of the Handle%d object. */
    HandleIndex id{ nextIndex++ }; /*!< The indexed location of the Handle%d object. */

    Handle() {}
    Handle(HandleSize size) : size{ size } {}

    /*! Basic equality comparator. */
    bool operator==(const Handle &rhs) noexcept
    {
        return (size == rhs.size && id == rhs.id);
    }

  private:
    static std::size_t nextIndex;
};

struct HandleHash
{
    std::size_t operator()(const Handle &h) const noexcept
    {
        auto hash1 = std::hash<HandleSize>()(h.size);
        auto hash2 = std::hash<HandleIndex>()(h.id);
        return hash1 ^= hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
    }
};

struct HandleEqual
{
    bool operator()(const Handle &lhs, const Handle &rhs) const noexcept
    {
        return lhs.size == rhs.size && lhs.id == rhs.id;
    }
};

// ALIGNED ARRAY
inline void *aligned_malloc(size_t size, size_t align) noexcept
{
    void *result;
#ifdef _WIN32
    result = _aligned_malloc(size, align);
#else
    if (posix_memalign(&result, align, size)) result = 0;
#endif
    return result;
}

inline void aligned_free(void *ptr) noexcept
{
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

template <class T, std::size_t S, std::size_t A>
struct alignas(A) AlignedArray
{
  public:
    T *data() noexcept { return _array.data(); }
    std::size_t size() noexcept { return S; }
    std::size_t alignment() noexcept { return A; }

    T &operator[](std::size_t i) { return _array[i]; }
    void *operator new(std::size_t sz)
    {
        std::cout << "lol" << std::endl;
        return aligned_malloc(sz, A);
    }
    void operator delete(void *ptr) { return aligned_free(ptr); }

  private:
    std::array<T, S> _array{};
};

// UUID STRUCT 64 BITS
struct UUID64
{
    uint64_t uuid;

    UUID64()
    noexcept
    {
        std::random_device r;
        std::seed_seq s{ r(), r(), r(), r(), r(), r(), r(), r() };
        std::mt19937_64 e{ s };

        auto random = e();
        uuid = random;
    }

    UUID64(uint64_t id)
    noexcept : uuid{ id } {}

    bool operator==(const UUID64 &rhs) const noexcept
    {
        return uuid == rhs.uuid;
    }

    bool operator!=(const UUID64 &rhs) const noexcept
    {
        return uuid != rhs.uuid;
    }
};

struct UUID64Cmp
{
    bool operator()(const UUID64 &lhs, const UUID64 &rhs) const
    {
        return lhs.uuid < rhs.uuid;
    }
};