#ifndef RZ_FRAMEWORK_OBJECTPOOL_HPP
#define RZ_FRAMEWORK_OBJECTPOOL_HPP

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
#include <cstddef>

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
namespace rz::objectpool
{
  namespace error
  {
      class InvalidPointer : public std::invalid_argument
      {
        public:
          InvalidPointer()
              : std::invalid_argument{ getMessage() } {}

        private:
          static std::string getMessage()
          {
              std::stringstream message;
              message << "Internal Error: ptr not found in Pool.";

              return message.str();
          }
      };

      class HandleOutOfRange : public std::out_of_range
      {
        public:
          HandleOutOfRange(const Handle &h, std::size_t s)
              : std::out_of_range{ getMessage(h, s) } {}

        private:
          static std::string getMessage(const Handle &h, std::size_t s)
          {
              std::stringstream message;
              message << "User Error: Handle{ type: " << h.type << ", id: " << h.id << " }"
                      << " not found in Pool<" << s << ">.";

              return message.str();
          }
      };

      class TypeMismatch : public std::invalid_argument
      {
        public:
          TypeMismatch(const Handle &handle, const std::type_info &type)
              : std::invalid_argument{ getMessage(handle, type) } {}

        private:
          static std::string getMessage(const Handle &handle, const std::type_info &type)
          {
              std::stringstream message;
              message << "User Error: Type mismatch, Handle::type{" << handle.type << "} != T::hash_code{ " << type.hash_code() << "}. T::name: " << type.name();

              return message.str();
          }
      };
  }

    /*! @cond */
    template <std::size_t S>
    class Page
    {
      public:
        struct alignas(S) Line
        {
            Line *next;
            std::size_t index;
        };

        using Array = AlignedArray<Line, OBJECT_POOL_PAGE_LENGTH, std::max(S, OBJECT_POOL_PAGE_ALIGNMENT)>;

        Page() : _data{}
        {
            begin = reinterpret_cast<std::size_t>(&_data.data()[0]);
            end = reinterpret_cast<std::size_t>(&_data.data()[OBJECT_POOL_PAGE_LENGTH - 1]);
        }

        constexpr std::size_t size() noexcept { return _data.size(); }
        auto data() noexcept { return _data.data(); }

        std::size_t begin, end;

      private:
        Array _data;
    };

	// Basically a freelist
    template <std::size_t S>
    class Pool
    {
        using PageType = Page<S>;
        using LineType = typename PageType::Line;
        using PageVector = std::vector<std::unique_ptr<PageType>>;
        using HandleMap = std::unordered_map<Handle, void *, HandleHash, HandleEqual>;

      public:
        template <typename T>
        Handle push(T &&object);

        template <typename T, typename... Args>
        Handle emplace(Args... args);

        template <typename T>
        T *get(const Handle &handle);

        template <typename T>
        void erase(const Handle &handle);

        void defragment();

        void shrink();

        std::size_t capacity()
        {
            if (_pages.size())
                return _pages.size() * sizeof(typename PageType::Array);
            else
                return 0;
        }

      private:
        void addPage();
        std::size_t getIndex(void *ptr);

        template <typename T, typename Object = typename std::decay<T>::type>
        Object *pushImpl(T &&object);

        template <typename T>
        T *getImpl(const Handle &handle);

        template <typename T>
        void eraseImpl(const Handle &handle, bool destruct);

        PageVector _pages{};
        LineType *_firstFreeLine{ nullptr };
        HandleMap _map{};
        std::mutex _mutex{};
    };
    /*! @endcond */

    //PUBLIC FUNCTIONS
    template <std::size_t S>
    template <typename T>
    inline Handle Pool<S>::push(T &&object)
    {
        std::scoped_lock lk{ _mutex };

        Handle h{ typeid(T).hash_code() };

        _map[h] = pushImpl(std::forward<T>(object));

        return h;
    }

    template <std::size_t S>
    template <typename T, typename... Args>
    inline Handle Pool<S>::emplace(Args... args)
    {
        std::scoped_lock lk{ _mutex };

        // If the next free position pointer points to non-existant page, add a new page
        if (_firstFreeLine == nullptr || _firstFreeLine->next == nullptr)
        {
            addPage();
        }

        // Get pointers to the current and next free lines
        LineType *curFree = _firstFreeLine;
        LineType *nextFree = _firstFreeLine->next;

        // Construct object to the location of current free pointer
        new (curFree) T{ args... };

        // Set the pools first free pointer to the next free pointer
        _firstFreeLine = nextFree;

        Handle h{ typeid(T).hash_code() };
        _map[h] = curFree;

        return h;
    }

    template <std::size_t S>
    template <typename T>
    inline T *Pool<S>::get(const Handle &handle)
    {
        std::scoped_lock lk{ _mutex };

        return getImpl<T>(handle);
    }

    template <std::size_t S>
    template <typename T>
    inline void Pool<S>::erase(const Handle &handle)
    {
        std::scoped_lock lk{ _mutex };

        eraseImpl<T>(handle, true);

        if (!_map.erase(handle))
            throw error::HandleOutOfRange{ handle, S };

        return;
    }

    template <std::size_t S>
    inline void Pool<S>::defragment()
    {
        std::scoped_lock lk{ _mutex };

        using Buffer = std::array<std::byte, S>;

        int count = 0;
        for (auto & [ hdl, ptr ] : _map)
        {
            if (getIndex(ptr) > _firstFreeLine->index)
            {
                count++;

                Buffer data = *static_cast<Buffer *>(ptr);

                eraseImpl<Buffer>(hdl, false);
                ptr = pushImpl(std::move(data));
            }
        }
    }

    template <std::size_t S>
    inline void Pool<S>::shrink()
    {
        std::scoped_lock lk{ _mutex };

        if (_firstFreeLine == nullptr)
            return;

        std::vector<LineType *> freePtrs{ _firstFreeLine };

        std::size_t lastPos = _pages.size() * OBJECT_POOL_PAGE_LENGTH - 1;

        // loop through all free lines
        while (freePtrs.back()->next != nullptr)
        {
            freePtrs.push_back(freePtrs.back()->next);
        }

        // return early if not enough free lines
        if (freePtrs.size() < OBJECT_POOL_PAGE_LENGTH)
            return;

        lastPos++;
        size_t pos = freePtrs.size();
        size_t toDelete = 0;
        while (pos > 0)
        {
            pos -= OBJECT_POOL_PAGE_LENGTH;

            if (freePtrs[pos]->index == (lastPos -= OBJECT_POOL_PAGE_LENGTH))
                toDelete++;
            else
                break;
        }

        // If _firstFreeLine is in an unwanted page, set it to nullptr
        if (_firstFreeLine->index >= (_pages.size() - toDelete) * OBJECT_POOL_PAGE_LENGTH)
            _firstFreeLine = nullptr;

        // Erase unwanted pages
        auto it = _pages.begin();
        std::advance(it, _pages.size() - toDelete);

        _pages.erase(it, _pages.end());
    }

    //PRIVATE FUNCTIONS
    template <std::size_t S>
    inline void Pool<S>::addPage()
    {
        auto page = new PageType;

        // Initialize the pages lines with pointers pointing to the next free line
        auto i = 0u;
        for (; i < OBJECT_POOL_PAGE_LENGTH - 1; i++)
        {
            page->data()[i].next = &page->data()[i + 1];
            page->data()[i].index = _pages.size() * OBJECT_POOL_PAGE_LENGTH + i;
        }

        // Last line points to nullptr
        page->data()[i].next = nullptr;

        // If first page, set _freePtr to the new pages first free line
        if (_firstFreeLine == nullptr)
            _firstFreeLine = &page->data()[0];
        // Else set the last pages last line to the new pages first line
        else
            _pages.back()->data()[i].next = &page->data()[0];

        _pages.emplace_back(page);
    }

    template <std::size_t S>
    inline std::size_t Pool<S>::getIndex(void *ptr)
    {
        std::size_t pos = reinterpret_cast<std::size_t>(ptr);
        PageType *page = nullptr;
        std::size_t pageNum = 0;
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

        throw error::InvalidPointer{};
    }

    template <std::size_t S>
    template <typename T, typename Object>
    inline Object *Pool<S>::pushImpl(T &&object)
    {
        // If the next free position pointer points to non-existant page, add a new page
        if (_firstFreeLine == nullptr || _firstFreeLine->next == nullptr)
        {
            addPage();
        }

        // Get pointers to the current and next free lines
        LineType *curFree = _firstFreeLine;
        LineType *nextFree = _firstFreeLine->next;

        // Copy object data to the location current free pointer
        std::memcpy(curFree, &object, sizeof(Object));

        // Set the pools first free pointer to the next free pointer
        _firstFreeLine = nextFree;

        return reinterpret_cast<Object *>(curFree);
    }

    template <std::size_t S>
    template <typename T>
    inline T *Pool<S>::getImpl(const Handle &handle)
    {
        auto it = _map.find(handle);

        if (it != _map.end())
            return static_cast<T *>(it->second);
        else
            throw error::HandleOutOfRange{ handle, S };
    }

    template <std::size_t S>
    template <typename T>
    inline void Pool<S>::eraseImpl(const Handle &handle, bool destruct)
    {
        if (destruct)
            std::destroy_at(getImpl<T>(handle));

        LineType *ptrToRemove = getImpl<LineType>(handle);

        // If the object being removed is located BEFORE the first free position
        if (getIndex(ptrToRemove) < getIndex(_firstFreeLine))
        {
            ptrToRemove->next = _firstFreeLine;
            ptrToRemove->index = getIndex(ptrToRemove);

            _firstFreeLine = ptrToRemove;
        }
        // If the object being removed is located AFTER the first free position
        else
        {
            LineType *prevFree = nullptr;
            LineType *nextFree = _firstFreeLine;

            std::size_t pos = getIndex(ptrToRemove);

            // Loop through free positions until ptrToRemove is inbetween prevFree and nextFree
            while (getIndex(nextFree) < pos)
            {
                prevFree = nextFree;
                nextFree = nextFree->next;
            }

            // Set ptrToRemove to be inbetween ptrPrevFree and ptrNextFree
            prevFree->next = ptrToRemove;
            ptrToRemove->next = nextFree;
            ptrToRemove->index = pos;
        }

        return;
    }

    /*!	Stores objects of any type with size upto `sizeof(std::size_t) * 64` Bytes in contiguous aligned memory.
	*   For more information and examples, see page \ref objectpool.
	*/
    class ObjectPool
    {
        /*! @cond */
        using PoolA = Pool<OBJECT_SIZE_2>;
        using PoolB = Pool<OBJECT_SIZE_4>;
        using PoolC = Pool<OBJECT_SIZE_8>;
        using PoolD = Pool<OBJECT_SIZE_16>;
        using PoolE = Pool<OBJECT_SIZE_32>;
        using PoolF = Pool<OBJECT_SIZE_64>;

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
        /*!	Pushes an object of type T into the ObjectPool.
		*
		*	@tparam		T				  The type of the object to be moved into the ObjectPool.<sup>[1]</sup>
		*
		*	@param		object			  The object to move into the ObjectPool.
		*
		*	@return		Handle			  A Handle for accessing the object.
		*
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
        template <typename T>
        Handle push(T &&object);

        /*!	In-place constructs an object of type T into the ObjectPool.
		*
		*	@tparam		T				   The type of the object to be constructed in the ObjectPool.
		*	@tparam		Args			   The parameter pack used to construct the T object.<sup>[1]</sup>
		*
		*	@param		args			   Constructor arguments to pass to the constructor of T.
		*
		*	@retval		Handle		       A Handle for accessing the object.
		*
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
        template <typename T, typename... Args>
        Handle emplace(Args... args);

        /*!	Gets a pointer to an object in the ObjectPool.
		*
		*	@tparam	T			               The type of the object to get from the ObjectPool.
		*
		*	@param	handle	                   The Handle used to search for the object in the ObjectPool.
		*
		*	@exception	std::invalid_argument  T and handle are mismatched.
		*
		*	@retval T*                         On success, a pointer to the desired object.
		*	@retval nullptr                    On failure, a nullptr.
		*/
        template <typename T>
        T *get(const Handle &handle);

        //TODO template<typename T> std::vector<T*> get(std::vector<Handle> handles);

        /*!	Removes an object from the ObjectPool and free's the space for reuse.
		*	It calls the destructor for non-trivially destructible objects.
		*
		*   @tparam	T			The type of the object to remove from the ObjectPool.
		*
		*	@param	Handle	The Handle of the object to remove from the ObjectPool.
		*/
        template <typename T>
        void erase(const Handle &handle);

		// TODO if shared/unique pointer is made, disable manual erase or do something else to avoid shenanigans
        /*!	Creates a unique pointer for automatic lifetime tracking of an already allocated object.
		*	Note: Destroying or reasigning the unique pointer will call `ObjectPool::erase` with the
		*	appropriate destructor. Also, manually calling `ObjectPool::erase` will invalidate the pointer.
		*
		*   @tparam	T						The type of the object to create a unique pointer for.
		*
		*	@param	Handle					The Handle of the object to create a unique pointer for.
		*
		*	@return	std::unique_ptr<Handle>	A unique pointer to the object in the ObjectPool.
		*/
        template <typename T>
        auto makeUnique(const Handle &h);

        /*!	Creates a shared pointer for automatic lifetime tracking of an already allocated object.
		*	Note: Destroying or reasigning the last shared pointer will call `ObjectPool::erase` with the
		*	appropriate destructor. Also, manually calling `ObjectPool::erase` will invalidate the pointers.
		*
		*   @tparam	T						The type of the object to create a shared pointer for.
		*
		*	@param	Handle					The Handle of the object to create a shared pointer for.
		*
		*	@return	std::shared_ptr<Handle>	A shared pointer to the object in the ObjectPool.
		*/
        template <typename T>
        auto makeShared(const Handle &h);

        /*! Defragments the ObjectPool such that objects located after the first free position are moved to earlier free positions. */
        void defragment();

        /*! Deletes unused pages from Pool%s. */
        void shrink();

        /*! Returns the currently used memory (in bytes) in total for all the Pool%s. */
        std::size_t capacity();

        /*! Returns the currently used memory (in bytes) for the Pool that fits `sizeof(T)`. */
        template <typename T>
        std::size_t capacity();

      private:
        template <typename... Pools>
        void defragmentImpl();

        template <typename... Pools>
        void shrinkImpl();

        template <typename... Pools>
        std::size_t capacityImpl();

        PoolTuple _pools{};
    };

    //PUBLIC FUNCTIONS
    template <typename T>
    inline Handle ObjectPool::push(T &&object)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        return pool.push(std::forward<T>(object));
    }

    template <typename T, typename... Args>
    inline Handle ObjectPool::emplace(Args... args)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        return pool.template emplace<T>(args...);
    }

    template <typename T>
    inline T *ObjectPool::get(const Handle &handle)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (handle.type != typeid(T).hash_code())
            throw error::TypeMismatch{ handle, typeid(T) };

        return pool.template get<T>(handle);
    }

    template <typename T>
    inline void ObjectPool::erase(const Handle &handle)
    {
        // Find the pool that fits T
        using Pool = typename PoolCond<T>::type;
        auto &pool = std::get<Pool>(_pools);

        if (handle.type != typeid(T).hash_code())
            throw error::TypeMismatch{ handle, typeid(T) };

        return pool.template erase<T>(handle);
    }

    template <typename T>
    auto ObjectPool::makeUnique(const Handle &handle)
    {
        if (handle.type != typeid(T).hash_code())
            throw error::TypeMismatch{ handle, typeid(T) };

        auto destroy = [&](Handle *h) {
            erase<T>(*h);
        };

        return std::unique_ptr<Handle, decltype(destroy)>(new Handle{ handle }, destroy);
      }

      template <typename T>
      auto ObjectPool::makeShared(const Handle &handle)
      {
        if (handle.type != typeid(T).hash_code())
            throw error::TypeMismatch{ handle, typeid(T) };

        auto destroy = [&](Handle *h) {
            erase<T>(*h);
        };

        return std::shared_ptr<Handle>(new Handle{ handle }, destroy);
    }

    inline void ObjectPool::defragment()
    {
        defragmentImpl<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>();
    }

    inline void ObjectPool::shrink()
    {
        shrinkImpl<PoolA, PoolB, PoolC, PoolD, PoolE, PoolF>();
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

    //PRIVATE FUNCTIONS
    template <typename... Pools>
    inline void ObjectPool::defragmentImpl()
    {
        return (std::get<Pools>(_pools).defragment(), ...);
    }

    template <typename... Pools>
    inline void ObjectPool::shrinkImpl()
    {
        return (std::get<Pools>(_pools).shrink(), ...);
    }

    template <typename... Pools>
    inline std::size_t ObjectPool::capacityImpl()
    {
        return (std::get<Pools>(_pools).capacity() + ...);
    }
}



#endif //RZ_FRAMEWORK_OBJECTPOOL_HPP
