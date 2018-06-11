#pragma once

#include "Misc.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <vector>

const std::size_t OBJECT_POOL_PAGE_LENGTH = 32;
const std::size_t OBJECT_POOL_PAGE_ALIGNMENT = 64;

const std::size_t OBJECT_SIZE_1 = sizeof(std::size_t);
const std::size_t OBJECT_SIZE_2 = sizeof(std::size_t) * 2;
const std::size_t OBJECT_SIZE_4 = sizeof(std::size_t) * 4;
const std::size_t OBJECT_SIZE_8 = sizeof(std::size_t) * 8;
const std::size_t OBJECT_SIZE_16 = sizeof(std::size_t) * 16;
const std::size_t OBJECT_SIZE_32 = sizeof(std::size_t) * 32;
const std::size_t OBJECT_SIZE_64 = sizeof(std::size_t) * 64;

/*! Things related to an aligned generic object pool implementation. */
namespace razaron::objectpool
{
    /*! Hashmap for mapping Handle%s to pointers. */
    using HandleMap = std::unordered_map<Handle, void *, HandleHash, HandleEqual>;

    // returns std::unique_ptr<T, decltype(destroy)>(T*, destroy)
    /*
	Handle push<T>(T &&obj)
	{
	//blabla
	return handle;
	}

	std::unique_ptr<T> make_unique(Handle h)
	{
	T* obj = getObject<T>(h);

	auto destroy = [&](Handle *obj){
	erase<T>(h);
	};

	returns std::unique_ptr<Handle, decltype(destroy)>(h*, destroy)
	}
	*/

    /***
	*    ______
	*    | ___ \
	*    | |_/ /_ _  __ _  ___
	*    |  __/ _` |/ _` |/ _ \
	*    | | | (_| | (_| |  __/
	*    \_|  \__,_|\__, |\___|
	*                __/ |
	*               |___/
	*/
    template <std::size_t S>
    class Page
    {
      public:
        struct Line
        {
            Line *next;

          private:
            std::array<std::byte, S - sizeof(Line::next)> padding;
        };

        using ArrayType = AlignedArray<Line, OBJECT_POOL_PAGE_LENGTH, OBJECT_POOL_PAGE_ALIGNMENT>;

        Page() : _data{}
        {
            begin = reinterpret_cast<std::size_t>(&_data.data()[0]);
            end = reinterpret_cast<std::size_t>(&_data.data()[OBJECT_POOL_PAGE_LENGTH * S - 1]);
        }

        constexpr std::size_t size() { return _data.size(); }
        auto data() { return _data.data(); }

        std::size_t begin, end;

      private:
        ArrayType _data;
    };

    /***
	*    ______             _     _     _
	*    |  ___|           | |   (_)   | |
	*    | |_ _ __ ___  ___| |    _ ___| |_
	*    |  _| '__/ _ \/ _ \ |   | / __| __|
	*    | | | | |  __/  __/ |___| \__ \ |_
	*    \_| |_|  \___|\___\_____/_|___/\__|
	*
	*
	*/
    template <std::size_t S>
    class FreeList
    {
        using PageType = Page<S>;
        using LineType = typename PageType::Line;
        using PageVector = std::vector<std::unique_ptr<PageType>>;

      public:
        template <class T>
        Handle push(T &&object);

        template <class T, class... Args>
        Handle emplace(Args... args);

        template <class T>
        T *get(const Handle &handle);

        template <class T>
        void erase(const Handle &handle);

        std::size_t capacity()
        {
            if (_pages.size())
                return _pages.size() * sizeof(typename PageType::ArrayType);
            else
                return 0;
        }

      private:
        void addPage();
        typename std::size_t getPosition(LineType *ptr);

        PageVector _pages{};
        LineType *_freeLinePtr{ nullptr };
        HandleMap _map{};
        std::mutex _mutex{};
    };

    /* *************************************************
	PUBLIC FUNCTIONS
	****************************************************/
    template <std::size_t S>
    template <class T>
    inline Handle FreeList<S>::push(T &&object)
    {
        // If the next free position pointer points to non-existant page, add a new page
        if (_freeLinePtr == nullptr || _freeLinePtr->next == nullptr)
        {
            addPage();
        }

        // Get pointers to the current and next free lines
        LineType *curFree = _freeLinePtr;
        LineType *nextFree = _freeLinePtr->next;

        // Copy object data to the location current free pointer
        std::memcpy(curFree, &object, sizeof(T));

        // Set the pools first free pointer to the next free pointer
        _freeLinePtr = nextFree;

        Handle h{ sizeof(T) };
        _map[h] = curFree;

        return h;
    }

    template <std::size_t S>
    template <class T, class... Args>
    inline Handle FreeList<S>::emplace(Args... args)
    {
        // If the next free position pointer points to non-existant page, add a new page
        if (_freeLinePtr == nullptr || _freeLinePtr->next == nullptr)
        {
            addPage();
        }

        // Get pointers to the current and next free lines
        LineType *curFree = _freeLinePtr;
        LineType *nextFree = _freeLinePtr->next;

        // Construct object to the location of current free pointer
        auto ptr = new (curFree) T{ args... };

        // Set the pools first free pointer to the next free pointer
        _freeLinePtr = nextFree;

        Handle h{ sizeof(T) };
        _map[h] = curFree;

        return h;
    }

    template <std::size_t S>
    template <class T>
    inline T *FreeList<S>::get(const Handle &handle)
    {
        auto it = _map.find(handle);

        if (it != _map.end())
        {
            return static_cast<T *>(it->second);
        }
        else
            return nullptr;
    }

    template <std::size_t S>
    template <class T>
    inline void FreeList<S>::erase(const Handle &handle)
    {
        std::destroy_at(get<T>(handle));

        LineType *ptrToRemove = get<LineType>(handle);

        // If the object being removed is located BEFORE the first free position
        if (getPosition(ptrToRemove) < getPosition(_freeLinePtr))
        {
            ptrToRemove->next = _freeLinePtr;

            _freeLinePtr = ptrToRemove;
        }
        // If the object being removed is located AFTER the first free position
        else
        {
            LineType *prevFree = nullptr;
            LineType *nextFree = _freeLinePtr;

            std::size_t pos = getPosition(ptrToRemove);

            // Loop through free positions until ptrToRemove is inbetween prevFree and nextFree
            while (getPosition(nextFree) < pos)
            {
                prevFree = nextFree;
                nextFree = nextFree->next;
            }

            // Set ptrToRemove to be inbetween ptrPrevFree and ptrNextFree
            prevFree->next = ptrToRemove;
            ptrToRemove->next = nextFree;
        }

        if (!_map.erase(handle))
        {
            std::stringstream message;
            message << "Handle{ size: " << handle.size << ", index: " << handle.index << " }"
                    << " not found in ObjectPool::_hashMap.";

            throw std::out_of_range(message.str());
        }
        return;
    }

    /***************************************************
	PRIVATE FUNCTIONS
	****************************************************/
    template <std::size_t S>
    inline void FreeList<S>::addPage()
    {
        auto page = new PageType;

        // Initialize the pages lines with pointers pointing to the next free line
        auto i = 0;
        for (; i < OBJECT_POOL_PAGE_LENGTH - 1; i++)
        {
            page->data()[i].next = &page->data()[i + 1];
        }

        // Last line points to nullptr
        page->data()[i].next = nullptr;

        // If first page, set _freePtr to the new pages first free line
        if (_freeLinePtr == nullptr)
            _freeLinePtr = &page->data()[0];
        // Else set the last pages last line to the new pages first line
        else
            _pages.back()->data()[i].next = &page->data()[0];

        _pages.emplace_back(page);
    }

    template <std::size_t S>
    inline std::size_t FreeList<S>::getPosition(LineType *ptr)
    {
        std::size_t pos = reinterpret_cast<std::size_t>(ptr);
        PageType *page = nullptr;
        int pageNum = 0;
        for (auto &p : _pages)
        {
            if (pos >= p->begin && pos <= p->end)
            {
                page = p.get();
                break;
            }
            pageNum++;
        }

        if (page)
        {
            std::size_t offset = pos - page->begin;
            offset /= sizeof(LineType);

            return pageNum * OBJECT_POOL_PAGE_LENGTH + offset;
        }

        std::stringstream message;
        message << "ptr not found in FreeList.";

        throw std::invalid_argument(message.str());
    }

    /***
	*     _____ _     _           _  ______           _
	*    |  _  | |   (_)         | | | ___ \         | |
	*    | | | | |__  _  ___  ___| |_| |_/ /__   ___ | |
	*    | | | | '_ \| |/ _ \/ __| __|  __/ _ \ / _ \| |
	*    \ \_/ / |_) | |  __/ (__| |_| | | (_) | (_) | |
	*     \___/|_.__/| |\___|\___|\__\_|  \___/ \___/|_|
	*               _/ |
	*              |__/
	*/

    /*!	Stores objects of any type with size upto \c sizeof(std::size_t)*64 Bytes in contiguous aligned memory.
	*   For more information and examples, see page \ref objectpool.
	*/
    class ObjectPool
    {
        /*! @cond */
        using PoolA = FreeList<OBJECT_SIZE_2>;
        using PoolB = FreeList<OBJECT_SIZE_4>;
        using PoolC = FreeList<OBJECT_SIZE_8>;
        using PoolD = FreeList<OBJECT_SIZE_16>;
        using PoolE = FreeList<OBJECT_SIZE_32>;
        using PoolF = FreeList<OBJECT_SIZE_64>;

        using PoolTuple = std::tuple<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>;

        // clang-format off
		template <typename T>
		struct PoolCond {
			static_assert(sizeof(T) <= OBJECT_SIZE_64, "T does not fit any pool.");

			using Cond = std::conditional <sizeof(T) <= OBJECT_SIZE_2, PoolA,
				typename std::conditional <sizeof(T) <= OBJECT_SIZE_4, PoolB,
				typename std::conditional <sizeof(T) <= OBJECT_SIZE_8, PoolC,
				typename std::conditional <sizeof(T) <= OBJECT_SIZE_16, PoolD,
				typename std::conditional <sizeof(T) <= OBJECT_SIZE_32, PoolE, PoolF>::type>::type>::type>::type>;
            // clang-format on

            using type = typename Cond::type;
        };
        /*! @endcond */

      public:
        /*!	Copies an object of type T into the ObjectPool.
		*
		*	@tparam		T				   The type of the object to be moved int o the ObjectPool.
		*
		*	@param		object		   The object to copy into the ObjectPool.
		*
		*	@exception	std::length_error  T is too large for ObjectPool.
		*
		*	@retval		Handle             On success, a Handle for accessing the object.
		*	@retval		Handle             On failure, an empty Handle.
		*/
        template <class T>
        Handle push(const T &object);

        /*!	Moves an object of type T into the ObjectPool.
		*
		*	@tparam		T				   The type of the object to be moved int o the ObjectPool.
		*
		*	@param		object		   The object to move into the ObjectPool.
		*
		*	@exception	std::length_error  T is too large for ObjectPool.
		*
		*	@retval		Handle             On success, a Handle for accessing the object.
		*	@retval		Handle             On failure, an empty Handle.
		*/
        template <class T>
        Handle push(T &&object);

        /*!	Constructs an object of type T directly into the ObjectPool.
		*
		*	@tparam		T				   The type of the object to be moved into the ObjectPool.
		*	@tparam		Args			   The parameter pack used to construct the T object.<sup>[1]</sup>
		*
		*	@param		args			   Constructor arguments to pass to the constructor of T.
		*
		*	@exception	std::length_error  T is too large for ObjectPool.
		*
		*	@retval		Handle		       On success, a Handle for accessing the object.
		*	@retval		Handle			   On failure, an empty Handle.
		*
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
        template <class T, class... Args>
        Handle emplace(Args... args);

        /*!	Gets a pointer to an object in the ObjectPool.
		*
		*	@tparam	T			               The type of the object to get from the ObjectPool.
		*
		*	@param	handle	                   The Handle used to search for the object in the ObjectPool.
		*
		*	@exception	std::invalid_argument  T and handle are mismatched.
		*	@exception	std::length_error      T is too large for ObjectPool.
		*
		*	@retval T*                         On success, a pointer to the desired object.
		*	@retval nullptr                    On failure, a nullptr.
		*/
        template <class T>
        T *get(const Handle &handle);

        //TODO template<class T> std::vector<T*> get(std::vector<Handle> handles);

        /*!	Removes an object from the ObjectPool and free's the space for reuse.
		*	It calls the destructor for non-trivially destructible objects.
		*
		*   @tparam	T			The type of the object to remove from the ObjectPool.
		*
		*	@param	handle	The Handle of the object to remove from the ObjectPool.
		*/
        template <class T>
        void erase(const Handle &handle);

        /*! Returns the current total capacity in bytes. */
        std::size_t capacity();

        /*! Returns the current capacity for the pool that fits sizeof(T) in bytes. */
        template <typename T>
        std::size_t capacity();

      private:
        template <typename... Pools>
        std::size_t capacityImpl();

        PoolTuple _pools{};
    };

    /* *************************************************
	PUBLIC FUNCTIONS
	****************************************************/
    template <class T>
    inline Handle ObjectPool::push(const T &object)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        T val = object;

        return pool.push<T>(std::move(val));
    }

    template <class T>
    inline Handle ObjectPool::push(T &&object)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        return pool.push<T>(std::forward<T>(object));
    }

    template <class T, class... Args>
    inline Handle ObjectPool::emplace(Args... args)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        return pool.emplace<T>(args...);
    }

    template <class T>
    inline T *ObjectPool::get(const Handle &handle)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (handle.size != sizeof(T))
        {
            std::stringstream message;
            message << "Type mismatch. HandleSize: " << handle.size << " != sizeof(T): " << sizeof(T) << ". typeid(T): " << typeid(T).name();

            throw std::invalid_argument(message.str());
        }

        return pool.get<T>(handle);
    }

    template <class T>
    inline void ObjectPool::erase(const Handle &handle)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (handle.size != sizeof(T))
        {
            std::stringstream message;
            message << "Type mismatch. HandleSize: " << handle.size << " != sizeof(T): " << sizeof(T) << ". typeid(T): " << typeid(T).name();

            throw std::invalid_argument(message.str());
        }

        pool.erase<T>(handle);

        return;
    }

    inline std::size_t ObjectPool::capacity()
    {
        return capacityImpl<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>();
    }

    template <typename T>
    inline std::size_t ObjectPool::capacity()
    {
        using Pool = typename PoolCond<T>::type;

        return capacityImpl<Pool>();
    }

    /* *************************************************
	PRIVATE FUNCTIONS
	****************************************************/
    template <typename... Pools>
    inline std::size_t ObjectPool::capacityImpl()
    {
        return (std::get<Pools>(_pools).capacity() + ...);
    }
}