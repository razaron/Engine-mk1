#pragma once

#ifndef OBJECT_POOL_PAGE_LENGTH
#define OBJECT_POOL_PAGE_LENGTH 32
#endif
#ifndef OBJECT_POOL_PAGE_ALIGNMENT
#define OBJECT_POOL_PAGE_ALIGNMENT 64
#endif

#define OBJECT_SIZE_1 sizeof(std::size_t) * 2

#define OBJECT_SIZE_2 sizeof(std::size_t) * 2
#define OBJECT_SIZE_4 sizeof(std::size_t) * 4
#define OBJECT_SIZE_8 sizeof(std::size_t) * 8
#define OBJECT_SIZE_16 sizeof(std::size_t) * 16
#define OBJECT_SIZE_32 sizeof(std::size_t) * 32
#define OBJECT_SIZE_64 sizeof(std::size_t) * 64

#include <cstring>
#include <list>
#include <sstream>
#include <tuple>
#include <typeinfo>
#include <mutex>
#include <memory>

#include "Misc.hpp"

/*! Things related to an aligned generic object pool implementation. */
namespace razaron::objectpool
{
    /*! @cond */
    using ArrayA = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_2, OBJECT_POOL_PAGE_ALIGNMENT>;
    using ArrayB = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_4, OBJECT_POOL_PAGE_ALIGNMENT>;
    using ArrayC = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_8, OBJECT_POOL_PAGE_ALIGNMENT>;
    using ArrayD = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_16, OBJECT_POOL_PAGE_ALIGNMENT>;
    using ArrayE = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_32, OBJECT_POOL_PAGE_ALIGNMENT>;
    using ArrayF = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * OBJECT_SIZE_64, OBJECT_POOL_PAGE_ALIGNMENT>;

    using PoolA = std::tuple<Handle *, std::list<ArrayA *>, std::shared_ptr<std::recursive_mutex>>;
    using PoolB = std::tuple<Handle *, std::list<ArrayB *>, std::shared_ptr<std::recursive_mutex>>;
    using PoolC = std::tuple<Handle *, std::list<ArrayC *>, std::shared_ptr<std::recursive_mutex>>;
    using PoolD = std::tuple<Handle *, std::list<ArrayD *>, std::shared_ptr<std::recursive_mutex>>;
    using PoolE = std::tuple<Handle *, std::list<ArrayE *>, std::shared_ptr<std::recursive_mutex>>;
    using PoolF = std::tuple<Handle *, std::list<ArrayF *>, std::shared_ptr<std::recursive_mutex>>;

    using PoolTuple = std::tuple<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>;

    // clang-format off
    template <typename T>
    using PoolCond1 =   std::conditional <sizeof(T) <= OBJECT_SIZE_2, PoolA,
                        typename std::conditional <sizeof(T) <= OBJECT_SIZE_4, PoolB,
                        typename std::conditional <sizeof(T) <= OBJECT_SIZE_8, PoolC,
                        typename std::conditional <sizeof(T) <= OBJECT_SIZE_16, PoolD,
                        typename std::conditional <sizeof(T) <= OBJECT_SIZE_32, PoolE, PoolF>::type>::type>::type>::type>;
    // clang-format on
    /*! @endcond */

    /*!	Stores objects of any type with size upto \c sizeof(std::size_t)*64 Bytes in contiguous aligned memory.
    *   For more information and examples, see page \ref objectpool.
    */
    class ObjectPool
    {
      public:
        ObjectPool();  /*!< Default constructor. */
        ~ObjectPool(); /*!< Default destructor. */

        /*!	Moves an object of type T into the ObjectPool.
		*
		*	@tparam		T				   The type of the object to be moved int o the ObjectPool.
		*
		*	@param		p_object		   The object to move into the ObjectPool.
		*
		*	@exception	std::length_error  T is too large for ObjectPool.
		*
		*	@retval		Handle             On success, a handle for accessing the object.
		*	@retval		Handle             On failure, an empty handle.
		*/
        template <class T>
        Handle push(T &p_object);

        /*!	Constructs an object of type T directly into the ObjectPool.
		*
		*	@tparam		T				   The type of the object to be moved into the ObjectPool.
		*	@tparam		Args			   The parameter pack used to construct the T object.<sup>[1]</sup>
		*
		*	@param		p_args			   Constructor arguments to pass to the constructor of T.
		*
		*	@exception	std::length_error  T is too large for ObjectPool.
		*
		*	@retval		Handle		       On success, a handle for accessing the object.
		*	@retval		Handle			   On failure, an empty handle.
		*
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
        template <class T, class... Args>
        Handle emplace(Args... p_args);

        /*!	Gets a pointer to an object in the ObjectPool.
		*
		*	@tparam	T			               The type of the object to get from the ObjectPool.
		*
        *	@param	p_handle	               The handle used to search for the object in the ObjectPool.
		*	@param	p_isNaive	               If true, check if p_handle belongs to a free position.
		*
        *	@exception	std::invalid_argument  T and p_handle are mismatched.
		*	@exception	std::length_error      T is too large for ObjectPool.
        *
        *	@retval T*                         On success, a pointer to the desired object.
		*	@retval nullptr                    On failure, a nullptr.
		*/
        template <class T>
        T *get(Handle p_handle, bool p_isNaive = false);
        //TODO template<class T> T* getObjects(std::vector<Handle> p_handles);

        /*!	Removes an object from the ObjectPool and free's the space for use.
		*	It calls the destructor for non-trivially destructible objects.
		*
        *   @tparam	T			The type of the object to remove from the ObjectPool.
        *
		*	@param	p_handle	The handle of the object to remove from the ObjectPool.
		*/
        template <class T>
        void erase(Handle p_handle);
        //TODO template<class T> void erase(Handle p_handle); // for calling the destructor ~T()
        //TODO void squash(); // Defragments the pools.
        HandleIndex capacity(); // added p_size parameter. Checks how many p_size bytes long object can fit.

      private:
        PoolTuple m_pools;

        template <class T, class Pool, class... Args>
        Handle allocateConstruct(Args... p_args);

        template <class T, class Pool>
        Handle allocateMove(T &&p_object);

        template <class Pool>
        void addPage();

        template <class Pool>
        typename std::tuple_element<1, Pool>::type::value_type getPage(HandleIndex p_index);

        template <class T, class Pool>
        T *getObject(Handle p_handle, bool p_isNaive = false);

        template <class Pool, typename T>
        typename std::enable_if<std::is_pointer<T>::value, HandleIndex>::type getIndex(T p_ptr);

        template <class T, class Pool>
        void erase(Handle p_handle);
    };

    /* *************************************************
					PUBLIC FUNCTIONS
	****************************************************/

    inline ObjectPool::ObjectPool()
    {
        m_pools = FOR_EACH_TUPLE({
            std::get<0>(element) = nullptr;
            std::get<2>(element) = std::make_shared<std::recursive_mutex>();

            return element;
        },
            m_pools);
    }

    inline ObjectPool::~ObjectPool()
    {
        m_pools = FOR_EACH_TUPLE({
            for (auto arr : std::get<1>(element))
                delete arr;

            return element;
        },
            m_pools);
    }

    template <class T>
    inline Handle ObjectPool::push(T &p_object)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;

        if (sizeof(T) <= OBJECT_SIZE_64)
        {
            return allocateMove<T, Pool>(std::move(p_object));
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
    inline Handle ObjectPool::emplace(Args... p_args)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;

        if (sizeof(T) <= OBJECT_SIZE_64)
        {
            return allocateConstruct<T, Pool>(p_args...);
        }
        else
        {
            std::stringstream message;
            message << typeid(T).name() << " is too large for ObjectPool. sizeof(" << typeid(T).name() << "): " << sizeof(T) << ".";

            throw std::length_error(message.str());
        }
    }

    template <class T>
    inline T *ObjectPool::get(Handle p_handle, bool p_isNaive)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;

        if (p_handle.size != sizeof(T))
        {
            std::stringstream message;
            message << "Type mismatch. HandleSize: " << p_handle.size << " != sizeof(T): " << sizeof(T) << ". typeid(T): " << typeid(T).name();

            throw std::invalid_argument(message.str());
        }
        else if (sizeof(T) <= OBJECT_SIZE_64)
        {
            return getObject<T, Pool>(p_handle, p_isNaive);
        }
        else
        {
            std::stringstream message;
            message << "HandleSize (" << p_handle.size << ") too large for ObjectPool.";

            throw std::length_error(message.str());
        }
    }

    template <class T>
    inline void ObjectPool::erase(Handle p_handle)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond1<T>::type;

        if (p_handle.size != sizeof(T))
        {
            std::stringstream message;
            message << "Type mismatch. HandleSize: " << p_handle.size << " != sizeof(T): " << sizeof(T) << ". typeid(T): " << typeid(T).name();

            throw std::invalid_argument(message.str());
        }
        else if (sizeof(T) <= OBJECT_SIZE_64)
        {
            return erase<T, Pool>(p_handle);
        }
        else
        {
            std::stringstream message;
            message << "HandleSize (" << p_handle.size << ") too large for ObjectPool.";

            throw std::length_error(message.str());
        }
    }

    inline HandleIndex ObjectPool::capacity()
    {
        auto pA = std::get<PoolA>(m_pools);
        auto pB = std::get<PoolB>(m_pools);
        auto pC = std::get<PoolC>(m_pools);
        auto pD = std::get<PoolD>(m_pools);
        auto pE = std::get<PoolE>(m_pools);
        auto pF = std::get<PoolF>(m_pools);

        return std::get<1>(pA).size() * sizeof(ArrayA) + std::get<1>(pB).size() * sizeof(ArrayB) + std::get<1>(pC).size() * sizeof(ArrayC) + std::get<1>(pD).size() * sizeof(ArrayD) + std::get<1>(pE).size() * sizeof(ArrayE) + std::get<1>(pF).size() * sizeof(ArrayF);
    }

    /* *************************************************
            		PRIVATE FUNCTIONS
	****************************************************/

    template <class T, class Pool, class... Args>
    inline Handle ObjectPool::allocateConstruct(Args... p_args)
    {
        T temp{p_args...};

        return allocateMove<T, Pool>(std::move(temp));
    }

    template <class T, class Pool>
    inline Handle ObjectPool::allocateMove(T &&p_object)
    {
        auto pool = &std::get<Pool>(m_pools);

        std::lock_guard<std::recursive_mutex> lk{*std::get<2>(*pool)};

        // If the next free position pointer points to non-existant page, add a new page
        int totalPositions = std::get<1>(*pool).size() * OBJECT_POOL_PAGE_LENGTH;
        if (totalPositions == 0 || totalPositions <= std::get<0>(*pool)->index)
        {
            addPage<Pool>();
        }

        // Get pointers to the current and next free elements
        Handle *curFree = std::get<0>(*pool);
        Handle *nextFree = getObject<Handle, Pool>(*curFree, true);

        // Copy object data to the location current free pointer
        std::memcpy(curFree, &p_object, sizeof(T));

        // Set the pools first free pointer to the next free pointer
        std::get<0>(*pool) = nextFree;

        // Configure a handle for the newly placed object
        Handle h{HandleSize{sizeof(T)}, HandleIndex{getIndex<Pool>(curFree)}, false};

        return h;
    }

    template <class Pool>
    inline void ObjectPool::addPage()
    {
        typedef typename std::tuple_element<1, Pool>::type::value_type PagePtr;
        typedef typename std::remove_pointer<PagePtr>::type Page;

        auto pool = &std::get<Pool>(m_pools);

        // Create and push a new page onto the pool
        auto page = new Page{};
        std::get<1>(*pool).push_back(page);

        // Initialize the pages positions with free handles pointing to the next free handle
        auto pageData = std::get<1>(*pool).back()->data();
        for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
        {
            HandleIndex nextFree = static_cast<HandleIndex>(i + 1 + ((std::get<1>(*pool).size() - 1) * OBJECT_POOL_PAGE_LENGTH));

            Handle h = {static_cast<HandleSize>(page->size() / OBJECT_POOL_PAGE_LENGTH), nextFree, true};
            std::memcpy(&pageData[i * page->size() / OBJECT_POOL_PAGE_LENGTH], &h, sizeof(h));
        }

        // If it's the first page, set the first free position to the beginning of the page
        if (std::get<0>(*pool) == nullptr)
            std::get<0>(*pool) = reinterpret_cast<Handle *>(page->data());
    }

    template <class Pool>
    inline typename std::tuple_element<1, Pool>::type::value_type ObjectPool::getPage(HandleIndex p_index)
    {
        typedef typename std::tuple_element<1, Pool>::type::value_type PagePtr;

        auto pool = &std::get<Pool>(m_pools);

        // Quotient is the page number and remainder is the position in that page
        std::div_t d = std::div(p_index, OBJECT_POOL_PAGE_LENGTH);

        // Finds a pointer to the correct page
        PagePtr page = nullptr;
        for (auto &p : std::get<1>(*pool))
        {
            if (!d.quot)
            {
                page = p;
                break;
            }
            d.quot--;
        }

        return page;
    }

    template <class T, class Pool>
    inline T *ObjectPool::getObject(Handle p_handle, bool p_isNaive)
    {
        typedef typename std::tuple_element<1, Pool>::type::value_type PagePtr;

        auto pool = &std::get<Pool>(m_pools);

        std::lock_guard<std::recursive_mutex> lk{*std::get<2>(*pool)};

        // Find the page containg p_handle
        PagePtr page = getPage<Pool>(p_handle.index);

        // Quotient is the page number and remainder is the position in that page
        std::div_t d = std::div(p_handle.index, OBJECT_POOL_PAGE_LENGTH);

        // Find and cast the element refering to objects first byte
        auto objectPtr = reinterpret_cast<T *>(&page->data()[d.rem * std::get<0>(*pool)->size]);

        // Loop through free pointers to see if objectPtr is one of them
        if (!p_isNaive && std::get<0>(*pool)->index <= p_handle.index)
        {
            Handle *freePtr = std::get<0>(*pool);
            while (freePtr->index <= p_handle.index)
            {
                freePtr = getObject<Handle, Pool>(
                    Handle{
                        HandleSize{freePtr->size},
                        HandleIndex{freePtr->index},
                        true},
                    true);
            }

            if (freePtr != reinterpret_cast<Handle *>(objectPtr))
                return objectPtr;
            else
                return nullptr;
        }
        else
            return objectPtr;
    }

    template <class Pool, typename T>
    inline typename std::enable_if<std::is_pointer<T>::value, HandleIndex>::type ObjectPool::getIndex(T p_ptr)
    {
        typedef typename std::tuple_element<1, Pool>::type::value_type PagePtr;
        typedef typename std::remove_pointer<PagePtr>::type Page;

        auto pool = &std::get<Pool>(m_pools);

        // Find the page that contains p_ptr
        std::size_t ptrAdr = reinterpret_cast<std::size_t>(p_ptr);
        std::size_t pageAdr = 0;
        std::size_t diff = 0;
		int pageNumber = 0;

        for (auto &p : std::get<1>(*pool))
        {
            pageAdr = reinterpret_cast<std::size_t>(p->data());
            diff = ptrAdr - pageAdr;

			++pageNumber;

            if (diff >= 0 && diff < sizeof(Page))
                break;
        }

        // Throw if no page found
        if (!(diff >= 0 && diff < sizeof(Page)))
        {
            throw std::out_of_range("Pointer is not in any page.");
        }

		// Calculate index relative to it's page
		std::size_t position = ptrAdr - pageAdr;
		position = position / std::get<0>(*pool)->size;

		// Add add sum of preceding positions to get absolute index
		position = position + (pageNumber - 1)*OBJECT_POOL_PAGE_LENGTH;

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

    template <class T, class Pool>
    inline void ObjectPool::erase(Handle p_handle)
    {
        auto pool = &std::get<Pool>(m_pools);

        std::lock_guard<std::recursive_mutex> lk{*std::get<2>(*pool)};

        // Get index of first free position
        auto posCurFree = getIndex<Pool>(std::get<0>(*pool));

        // Fail if first free position and object being removed are the same
        if (p_handle.index == posCurFree) return;

        Handle *ptrToRemove = getObject<Handle, Pool>(p_handle, true);

        // Call object destructor if it is manually set
        if(std::is_destructible<T>::value && !std::is_trivially_destructible<T>::value)
            reinterpret_cast<T*>(ptrToRemove)->~T();

        // Resets the data back to zero
        std::memset(ptrToRemove, 0, std::get<0>(*pool)->size);

        // If the object being removed is located BEFORE the first free position
        if (p_handle.index < posCurFree)
        {

            // Setup the object being removed to become the next firstFree pointer
            ptrToRemove->isFree = true;
            ptrToRemove->size = std::get<0>(*pool)->size;
            ptrToRemove->index = posCurFree;

            std::get<0>(*pool) = ptrToRemove;

            return;
        }

        // If the object being removed is located AFTER the first free position
        Handle *ptrPrevFree = nullptr;
        Handle *ptrNextFree = std::get<0>(*pool);

        std::size_t posNextFree = getIndex<Pool>(ptrNextFree);

        // Loop through free positions until p_handle is inbetween prevFree and nextFree
        while (posNextFree < p_handle.index)
        {
            ptrPrevFree = ptrNextFree;

            ptrNextFree = getObject<Handle, Pool>(*ptrNextFree, true);
            posNextFree = getIndex<Pool>(ptrNextFree);
        }

        // Currently, ptrToRemove is set to some value (e.g. "hello"), so I have to use p_handle
        ptrPrevFree->index = p_handle.index;

        // Setup the ptr being removed to be inbetween ptrPrevFree and ptrNextFree
        ptrToRemove->isFree = true;
		ptrToRemove->size = std::get<0>(*pool)->size;
        ptrToRemove->index = static_cast<HandleIndex>(posNextFree);


        return;
    }
}
