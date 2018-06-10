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
        using Data = AlignedArray<std::byte, OBJECT_POOL_PAGE_LENGTH * S, OBJECT_POOL_PAGE_ALIGNMENT>;

      public:
        constexpr std::size_t size() { return _data.size(); }
        auto data() { return _data.data(); }

      private:
        Data _data{};
        std::size_t _begin, _end;
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
                return _pages.size() * _pages[0]->size();
            else
                return 0;
        }

      private:
        template <class T>
        typename std::enable_if<std::is_pointer<T>::value, HandleIndex>::type getIndex(T ptr);

        void addPage();
        PageType *getPage(HandleIndex index);

        std::vector<std::unique_ptr<PageType>> _pages;
        Handle *_freePtr;

        std::recursive_mutex _mutex;
    };

    /* *************************************************
						PUBLIC FUNCTIONS
	****************************************************/
    template <std::size_t S>
    template <class T>
    inline Handle FreeList<S>::push(T &&object)
    {
        std::lock_guard<std::recursive_mutex> lk{ _mutex };

        // If the next free position pointer points to non-existant page, add a new page
        size_t totalPositions = _pages.size() * OBJECT_POOL_PAGE_LENGTH;
        if (totalPositions == 0 || totalPositions <= _freePtr->index)
        {
            addPage();
        }

        // Get pointers to the current and next free elements
        Handle *curFree = _freePtr;
        Handle *nextFree = get<Handle>(*curFree);

        // Copy object data to the location current free pointer
        std::memcpy(curFree, &object, sizeof(T));

        // Set the pools first free pointer to the next free pointer
        _freePtr = nextFree;

        // Configure a Handle for the newly placed object
        Handle h{ HandleSize{ sizeof(T) }, HandleIndex{ getIndex(curFree) } };

        return h;
    }

    template <std::size_t S>
    template <class T, class... Args>
    inline Handle FreeList<S>::emplace(Args... args)
    {
        std::lock_guard<std::recursive_mutex> lk{ _mutex };

        // If the next free position pointer points to non-existant page, add a new page
        size_t totalPositions = _pages.size() * OBJECT_POOL_PAGE_LENGTH;
        if (totalPositions == 0 || totalPositions <= _freePtr->index)
        {
            addPage();
        }

        // Get pointers to the current and next free elements
        Handle *curFree = _freePtr;
        Handle *nextFree = get<Handle>(*curFree);

        // Construct object to the location of current free pointer
        auto ptr = new (curFree) T{ args... };

        // Set the pools first free pointer to the next free pointer
        _freePtr = nextFree;

        // Configure a Handle for the newly placed object
        Handle h{ HandleSize{ sizeof(T) }, HandleIndex{ getIndex(curFree) } };

        return h;
    }

    template <std::size_t S>
    template <class T>
    inline T *FreeList<S>::get(const Handle &handle)
    {
        std::lock_guard<std::recursive_mutex> lk{ _mutex };

        // Find the page containg handle
        auto page = getPage(handle.index);

        // Quotient is the page number and remainder is the position in that page
        std::div_t d = std::div(handle.index, OBJECT_POOL_PAGE_LENGTH);

        // Find and cast the element refering to objects first byte
        auto objectPtr = reinterpret_cast<T *>(&page->data()[d.rem * _freePtr->size]);

        return objectPtr;
    }

    template <std::size_t S>
    template <class T>
    inline void FreeList<S>::erase(const Handle &handle)
    {
        std::lock_guard<std::recursive_mutex> lk{ _mutex };

        // Get index of first free position
        auto posCurFree = getIndex(_freePtr);

        // Fail if first free position and object being removed are the same
        if (handle.index == posCurFree) return;

        Handle *ptrToRemove = get<Handle>(handle);

        // Call object destructor if it is manually set
        std::destroy_at(reinterpret_cast<T *>(ptrToRemove));

        // Resets the data back to zero
        std::memset(ptrToRemove, 0, _freePtr->size);

        // If the object being removed is located BEFORE the first free position
        if (handle.index < posCurFree)
        {

            // Setup the object being removed to become the next firstFree pointer
            ptrToRemove->size = _freePtr->size;
            ptrToRemove->index = posCurFree;

            _freePtr = ptrToRemove;
        }
        // If the object being removed is located AFTER the first free position
        else
        {
            Handle *ptrPrevFree = nullptr;
            Handle *ptrNextFree = _freePtr;

            std::size_t posNextFree = getIndex(ptrNextFree);

            // Loop through free positions until handle is inbetween prevFree and nextFree
            while (posNextFree < handle.index)
            {
                ptrPrevFree = ptrNextFree;

                ptrNextFree = get<Handle>(*ptrNextFree);
                posNextFree = getIndex(ptrNextFree);
            }

            // Currently, ptrToRemove is zeroed, so I have to get it's index from handle
            ptrPrevFree->index = handle.index;

            // Setup the ptr being removed to be inbetween ptrPrevFree and ptrNextFree
            ptrToRemove->size = _freePtr->size;
            ptrToRemove->index = static_cast<HandleIndex>(posNextFree);
        }

        return;
    }

    /***************************************************
					PRIVATE FUNCTIONS
	****************************************************/
    template <std::size_t S>
    inline void FreeList<S>::addPage()
    {
        // Create and push a new page onto the pool
        auto page = new PageType;
        _pages.emplace_back(page);

        // Initialize the pages positions with free handles pointing to the next free Handle
        auto pageData = _pages.back()->data();
        for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
        {
            HandleIndex nextFree = static_cast<HandleIndex>(i + 1 + ((_pages.size() - 1) * OBJECT_POOL_PAGE_LENGTH));

            Handle h = { static_cast<HandleSize>(page->size() / OBJECT_POOL_PAGE_LENGTH), nextFree };
            std::memcpy(&pageData[i * page->size() / OBJECT_POOL_PAGE_LENGTH], &h, sizeof(h));
        }

        // If it's the first page, set the first free position to the beginning of the page
        if (_freePtr == nullptr)
            _freePtr = reinterpret_cast<Handle *>(page->data());
    }

    template <std::size_t S>
    inline typename FreeList<S>::PageType *FreeList<S>::getPage(HandleIndex index)
    {
        // Quotient is the page number and remainder is the position in that page
        std::div_t d = std::div(index, OBJECT_POOL_PAGE_LENGTH);

        // Finds a pointer to the correct page
        PageType *page = nullptr;
        for (auto &p : _pages)
        {
            if (!d.quot)
            {
                page = p.get();
                break;
            }
            d.quot--;
        }

        return page;
    }

    template <std::size_t S>
    template <class T>
    inline typename std::enable_if<std::is_pointer<T>::value, HandleIndex>::type FreeList<S>::getIndex(T ptr)
    {
        // Find the page that contains ptr
        std::size_t ptrAdr = reinterpret_cast<std::size_t>(ptr);
        std::size_t pageAdr = 0;
        std::size_t diff = 0;
        int pageNumber = 0;

        for (auto &p : _pages)
        {
            pageAdr = reinterpret_cast<std::size_t>(p->data());
            diff = ptrAdr - pageAdr;

            ++pageNumber;

            if (diff >= 0 && diff < sizeof(PageType))
                break;
        }

        // Throw if no page found
        if (!(diff >= 0 && diff < sizeof(PageType)))
        {
            throw std::out_of_range("Pointer is not in any page.");
        }

        // Calculate index relative to it's page
        std::size_t position = ptrAdr - pageAdr;
        position = position / _freePtr->size;

        // Add add sum of preceding positions to get absolute index
        position = position + (pageNumber - 1) * OBJECT_POOL_PAGE_LENGTH;

        // If position is in valid range, return. Else, throw.
        if (position <= std::numeric_limits<HandleIndex>::max())
        {
            return static_cast<HandleIndex>(position);
        }
        else
        {
            std::stringstream message;
            message << "Calculated position too large for HandleIndex max value. std::numeric_limits<HandleIndex>::max()" << std::numeric_limits<HandleIndex>::max();

            throw std::overflow_error(message.str());
        }
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
		using PoolCond1 = std::conditional <sizeof(T) <= OBJECT_SIZE_2, PoolA,
			typename std::conditional <sizeof(T) <= OBJECT_SIZE_4, PoolB,
			typename std::conditional <sizeof(T) <= OBJECT_SIZE_8, PoolC,
			typename std::conditional <sizeof(T) <= OBJECT_SIZE_16, PoolD,
			typename std::conditional <sizeof(T) <= OBJECT_SIZE_32, PoolE, PoolF>::type>::type>::type>::type>;
        // clang-format on
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
        std::size_t capacity(); // add overload with size parameter. Checks how many size bytes long object can fit.

      private:
        template <class T>
        T *getObject(const Handle &handle);

        template <typename... Pools>
        std::size_t capacityImpl();

        PoolTuple _pools{};
        HandleMap _hashMap{};
        std::mutex _hashMapMutex{};
    };

    /* *************************************************
	PUBLIC FUNCTIONS
	****************************************************/
    template <class T>
    inline Handle ObjectPool::push(const T &object)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;
        auto &pool = std::get<Pool>(_pools);

        T val = object;

        if (sizeof(T) <= OBJECT_SIZE_64)
        {
            auto h = pool.push<T>(std::move(val));

            // Adds the new object to the ObjectPools hashmap
            {
                std::lock_guard<std::mutex> lk{ _hashMapMutex };

                _hashMap[h] = static_cast<void *>(pool.get<T>(h));
            }

            return h;
        }
        else
        {
            std::stringstream message;
            message << typeid(T).name() << " is too large for ObjectPool. sizeof(" << typeid(T).name() << "): "
                    << ".";

            throw std::length_error(message.str());
        }
    }

    template <class T>
    inline Handle ObjectPool::push(T &&object)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (sizeof(T) <= OBJECT_SIZE_64)
        {
            auto h = pool.push<T>(std::forward<T>(object));

            // Adds the new object to the ObjectPools hashmap
            {
                std::lock_guard<std::mutex> lk{ _hashMapMutex };

                _hashMap[h] = static_cast<void *>(pool.get<T>(h));
            }

            return h;
        }
        else
        {
            std::stringstream message;
            message << typeid(T).name() << " is too large for ObjectPool. sizeof(" << typeid(T).name() << "): "
                    << ".";

            throw std::length_error(message.str());
        }
    }

    template <class T, class... Args>
    inline Handle ObjectPool::emplace(Args... args)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (sizeof(T) <= OBJECT_SIZE_64)
        {
            auto h = pool.emplace<T>(args...);

            // Adds the new object to the ObjectPools hashmap
            {
                std::lock_guard<std::mutex> lk{ _hashMapMutex };

                _hashMap[h] = static_cast<void *>(pool.get<T>(h));
            }

            return h;
        }
        else
        {
            std::stringstream message;
            message << typeid(T).name() << " is too large for ObjectPool. sizeof(" << typeid(T).name() << "): " << sizeof(T) << ".";

            throw std::length_error(message.str());
        }
    }

    template <class T>
    inline T *ObjectPool::get(const Handle &handle)
    {
        if (handle.size != sizeof(T))
        {
            std::stringstream message;
            message << "Type mismatch. HandleSize: " << handle.size << " != sizeof(T): " << sizeof(T) << ". typeid(T): " << typeid(T).name();

            throw std::invalid_argument(message.str());
        }
        else if (sizeof(T) <= OBJECT_SIZE_64)
        {
            return getObject<T>(handle);
        }
        else
        {
            std::stringstream message;
            message << "HandleSize (" << handle.size << ") too large for ObjectPool.";

            throw std::length_error(message.str());
        }
    }

    template <class T>
    inline void ObjectPool::erase(const Handle &handle)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (handle.size != sizeof(T))
        {
            std::stringstream message;
            message << "Type mismatch. HandleSize: " << handle.size << " != sizeof(T): " << sizeof(T) << ". typeid(T): " << typeid(T).name();

            throw std::invalid_argument(message.str());
        }
        else if (sizeof(T) <= OBJECT_SIZE_64)
        {
            pool.erase<T>(handle);

            // Removes object from the hashmap.
            {
                std::lock_guard<std::mutex> lk{ _hashMapMutex };

                if (!_hashMap.erase(handle))
                {
                    std::stringstream message;
                    message << "Handle{ size: " << handle.size << ", index: " << handle.index << " }"
                            << " not found in ObjectPool::_hashMap.";

                    throw std::out_of_range(message.str());
                }
            }

            return;
        }
        else
        {
            std::stringstream message;
            message << "HandleSize (" << handle.size << ") too large for ObjectPool.";

            throw std::length_error(message.str());
        }
    }

    inline std::size_t ObjectPool::capacity()
    {
        return capacityImpl<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>();
    }

    /* *************************************************
	PRIVATE FUNCTIONS
	****************************************************/
    template <class T>
    inline T *ObjectPool::getObject(const Handle &handle)
    {
        std::lock_guard<std::mutex> lk{ _hashMapMutex };

        auto it = _hashMap.find(handle);

        if (it != _hashMap.end())
        {
            return static_cast<T *>(it->second);
        }
        else
            return nullptr;
    }

    template <typename... Pools>
    inline std::size_t ObjectPool::capacityImpl()
    {
        return (std::get<Pools>(_pools).capacity() + ...);
    }
}