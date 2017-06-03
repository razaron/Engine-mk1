#pragma once

#ifndef OBJECT_POOL_PAGE_LENGTH
#define OBJECT_POOL_PAGE_LENGTH 32
#endif
#ifndef OBJECT_POOL_PAGE_ALIGNMENT
#define OBJECT_POOL_PAGE_ALIGNMENT 64
#endif

#include "Misc.hpp"

#include <list>
#include <tuple>
#include <sstream>
#include <typeinfo>
#include <cstring>

/*! Things related to an aligned generic object pool implementation. */
namespace razaron::objectpool
{
	/*! @cond */
	using Array8 = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * 8, OBJECT_POOL_PAGE_ALIGNMENT>;
	using Array16 = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * 16, OBJECT_POOL_PAGE_ALIGNMENT>;
	using Array32 = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * 32, OBJECT_POOL_PAGE_ALIGNMENT>;
	using Array64 = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * 64, OBJECT_POOL_PAGE_ALIGNMENT>;
	using Array128 = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * 128, OBJECT_POOL_PAGE_ALIGNMENT>;
	using Array256 = alignedArray<char, OBJECT_POOL_PAGE_LENGTH * 256, OBJECT_POOL_PAGE_ALIGNMENT>;

	using Pool8 = std::pair<Handle*, std::list<Array8*>>;
	using Pool16 = std::pair<Handle*, std::list<Array16*>>;
	using Pool32 = std::pair<Handle*, std::list<Array32*>>;
	using Pool64 = std::pair<Handle*, std::list<Array64*>>;
	using Pool128 = std::pair<Handle*, std::list<Array128*>>;
	using Pool256 = std::pair<Handle*, std::list<Array256*>>;
	/*! @endcond */

	/*!	Stores objects of any type with size upto 256 Bytes in contiguous aligned memory. */
	class ObjectPool {
	public:
		ObjectPool(); /*!< Default constructor. */
		~ObjectPool();/*!< Default destructor. */



		/*!	Moves an object of type T into the ObjectPool.
		*
		*	@tparam		T				The type of the object to be moved int o the ObjectPool.
		*
		*	@param		p_object		The object to move into the ObjectPool.
		*
		*	@exception	std::exception	T is too large for ObjectPool.
		*
		*	@retval		Handle			On success, a handle for accessing the object.
		*	@retval		Handle			On failure, an empty handle.
		*/
		template<class T> Handle push(T&& p_object);

		/*!	Constructs an object of type T directly into the ObjectPool.
		*
		*	@tparam		T				The type of the object to be moved into the ObjectPool.
		*	@tparam		Args			The parameter pack used to construct the T object.<sup>[1]</sup>
		*
		*	@param		p_args			Constructor arguments to pass to the constructor of T.
		*
		*	@exception	std::exception	T is too large for ObjectPool.
		*
		*	@retval		Handle			On success, a handle for accessing the object.
		*	@retval		Handle			On failure, an empty handle.
		*
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
		template<class T, class... Args> Handle emplace(Args... p_args);

		/*!	Gets a pointer to an object in the ObjectPool.
		*
		*	@tparam	T			The type of the object to get from the ObjectPool.
		*
		*	@param	p_handle	The handle used to search for the object in the ObjectPool.
		*
		*	@retval T*			On success, a pointer to the desired object.
		*/
		template<class T> T* getObject(Handle p_handle);
		//TODO template<class T> T* getObjects(std::vector<Handle> p_handles);

		/*!	Removes an object from the ObjectPool and free's the space for use.
		*	It does not call the objects destructor, it just resets the memory used to store the object in the ObjectPool.
		*
		*	@param	p_handle	The handle of the object to remove from the ObjectPool.
		*/
		void removeObject(Handle p_handle);
		//TODO template<class T> void removeObject(Handle p_handle); // for calling the destructor ~T()
		//TODO void squash(); // Defragments the pools.
		std::size_t capacity(); // added p_size parameter. Checks how many p_size bytes long object can fit.

	private:
		std::tuple<Pool8, Pool16, Pool32, Pool64, Pool128, Pool256> m_pools;

		template<class T, class Pool, class... Args> Handle allocateConstruct(Args... p_args);
		template<class T, class Pool> Handle allocateMove(T&& p_object);
		template <class Pool> void addPage();
		template<class T, class Pool> T* getObject(Handle p_handle);
		template<class Pool> void removeObject(Handle p_handle); //TODO only loop through list once
	};

	/* *************************************************
					PUBLIC FUNCTIONS
	****************************************************/

	inline ObjectPool::ObjectPool()
	{
		auto p8 = std::get<Pool8>(m_pools);
		p8.first = nullptr;

		auto p16 = std::get<Pool16>(m_pools);
		p16.first = nullptr;

		auto p32 = std::get<Pool32>(m_pools);
		p32.first = nullptr;

		auto p64 = std::get<Pool64>(m_pools);
		p64 .first = nullptr;

		auto p128 = std::get<Pool128>(m_pools);
		p128.first = nullptr;

		auto p256 = std::get<Pool256>(m_pools);
		p256.first = nullptr;
	}

	inline ObjectPool::~ObjectPool()
	{
		auto p8 = std::get<Pool8>(m_pools);
		for(auto arr: p8.second)
			delete arr;

		auto p16 = std::get<Pool16>(m_pools);
		for (auto arr : p16.second)
			delete arr;

		auto p32 = std::get<Pool32>(m_pools);
		for (auto arr : p32.second)
			delete arr;

		auto p64 = std::get<Pool64>(m_pools);
		for (auto arr : p64.second)
			delete arr;

		auto p128 = std::get<Pool128>(m_pools);
		for (auto arr : p128.second)
			delete arr;

		auto p256 = std::get<Pool256>(m_pools);
		for (auto arr : p256.second)
			delete arr;
	}

	template<class T>
	inline Handle ObjectPool::push(T && p_object)
	{
		if (sizeof(T) <= 8)
			return allocateMove<T, Pool8>(p_object);

		else if (sizeof(T) <= 16)
			return allocateMove<T, Pool16>(p_object);

		else if (sizeof(T) <= 32)
			return allocateMove<T, Pool32>(p_object);

		else if (sizeof(T) <= 64)
			return allocateMove<T, Pool64>(p_object);

		else if (sizeof(T) <= 128)
			return allocateMove<T, Pool128>(p_object);

		else if (sizeof(T) <= 256)
			return allocateMove<T, Pool256>(p_object);

		else {
			std::stringstream message;
			message << typeid(T).name() << " is too large for ObjectPool. sizeof(" << typeid(T).name() << "): " << sizeof(T);

			throw std::length_error(message.str());
		}

		return{};
	}

	template<class T, class... Args>
	inline Handle ObjectPool::emplace(Args... p_args)
	{
		if (sizeof(T) <= 8)
			return allocateConstruct<T, Pool8>(p_args...);

		else if (sizeof(T) <= 16)
			return allocateConstruct<T, Pool16>(p_args...);

		else if (sizeof(T) <= 32)
			return allocateConstruct<T, Pool32>(p_args...);

		else if (sizeof(T) <= 64)
			return allocateConstruct<T, Pool64>(p_args...);

		else if (sizeof(T) <= 128)
			return allocateConstruct<T, Pool128>(p_args...);

		else if (sizeof(T) <= 256)
			return allocateConstruct<T, Pool256>(p_args...);

		else {
			std::stringstream message;
			message << typeid(T).name() << " is too large for ObjectPool. sizeof(" << typeid(T).name() << "): " << sizeof(T);

			throw std::length_error(message.str());
		}

		return {};
	}

	inline void ObjectPool::removeObject(Handle p_handle)
	{
		if (p_handle.size <= 8)
			removeObject<Pool8>(p_handle);
		else if (p_handle.size <= 16)
			removeObject<Pool16>(p_handle);
		else if (p_handle.size <= 32)
			removeObject<Pool32>(p_handle);
		else if (p_handle.size <= 64)
			removeObject<Pool64>(p_handle);
		else if (p_handle.size <= 128)
			removeObject < Pool128 > (p_handle);
		else if (p_handle.size <= 256)
			removeObject<Pool256>(p_handle);
	}

	inline std::size_t ObjectPool::capacity()
	{
		auto p8 = std::get<Pool8>(m_pools);
		auto p16 = std::get<Pool16>(m_pools);
		auto p32 = std::get<Pool32>(m_pools);
		auto p64 = std::get<Pool64>(m_pools);
		auto p128 = std::get<Pool128>(m_pools);
		auto p256 = std::get<Pool256>(m_pools);

		return p8.second.size() * sizeof(Array8) + p16.second.size() * sizeof(Array16) + p32.second.size() * sizeof(Array32) + p64.second.size() * sizeof(Array64) + p128.second.size() * sizeof(Array128) + p256.second.size() * sizeof(Array256);
	}


	/* *************************************************
            		PRIVATE FUNCTIONS
	****************************************************/
	template<class T>
	inline T* ObjectPool::getObject(Handle p_handle)
	{
		if (sizeof(T) <= 8)
			return getObject<T, Pool8>(p_handle);
		else if (sizeof(T) <= 16)
			return getObject<T, Pool16>(p_handle);
		else if (sizeof(T) <= 32)
			return getObject<T, Pool32>(p_handle);
		else if (sizeof(T) <= 64)
			return getObject<T, Pool64>(p_handle);
		else if (sizeof(T) <= 128)
			return getObject<T, Pool128>(p_handle);
		else if (sizeof(T) <= 256)
			return getObject<T, Pool256>(p_handle);

		return nullptr;
	}

	template<class T, class Pool, class... Args>
	inline Handle ObjectPool::allocateConstruct(Args... p_args)
	{
		T temp = T(p_args...);

		return allocateMove<T, Pool>(std::move(temp));
	}

	template<class T, class Pool>
	inline Handle ObjectPool::allocateMove(T && p_object)
	{
		auto pool = &std::get<Pool>(m_pools);

		int totalChunks = pool->second.size() * OBJECT_POOL_PAGE_LENGTH;

		if (totalChunks == 0 || totalChunks <= pool->first->index)
		{
			addPage<Pool>();
		}

		std::div_t d = std::div(pool->first->index, OBJECT_POOL_PAGE_LENGTH);
		int count = d.quot;
		for (auto page : pool->second)
		{
			if (!count--)
			{
				Handle* a = pool->first;
				Handle* b = reinterpret_cast<Handle*>(page->data());
				std::ptrdiff_t index = (pool->first->index % OBJECT_POOL_PAGE_LENGTH) ? a - b : 0;

				int foo = (pool->first->size / (2 * sizeof(a)));

				foo = (foo) ? foo : 1;

				index = index / foo;

				std::memcpy(pool->first, &p_object, sizeof(T));

				pool->first = reinterpret_cast<Handle*>(&(page->data()[d.rem * page->size() / OBJECT_POOL_PAGE_LENGTH]));

				return{ sizeof(T), static_cast<unsigned short>(index), false };
			}
		}

		return{};
	}

	template<class Pool>
	inline void ObjectPool::addPage()
	{
		typedef typename std::remove_pointer<typename Pool::second_type::value_type>::type Page;

		auto pool = &std::get<Pool>(m_pools);

		auto page = new Page{};

		pool->second.push_back(page);

		auto pagePtr = pool->second.back()->data();

		for (auto i = 0; i < OBJECT_POOL_PAGE_LENGTH; i++)
		{
			unsigned short nextFree = static_cast<unsigned short>( i + 1 + ((pool->second.size() - 1)*OBJECT_POOL_PAGE_LENGTH) );

			Handle h = { page->size() / OBJECT_POOL_PAGE_LENGTH, nextFree, true };
			std::memcpy(&pagePtr[i * page->size() / OBJECT_POOL_PAGE_LENGTH], &h, sizeof(h));
		}

		if(pool->first == nullptr)
			pool->first = reinterpret_cast<Handle*>(page->data());
	}

	template<class T, class Pool>
	inline T * ObjectPool::getObject(Handle p_handle)
	{
		typedef typename Pool::second_type::value_type PagePtr;

		auto pool = &std::get<Pool>(m_pools);
		std::div_t d = std::div(p_handle.index, OBJECT_POOL_PAGE_LENGTH);

		PagePtr page = nullptr;

		for (auto &p : pool->second)
		{
			if (!d.quot)
			{
				page = p;
			}
			d.quot--;
		}

		int v = p_handle.size;
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;

		T* object = reinterpret_cast<T*>(&page->data()[p_handle.index*v]);

		// TODO if p_handle.size != sizeof(T) return nullptr

		return object;
	}

	template<class Pool>
	inline void ObjectPool::removeObject(Handle p_handle)
	{
		typedef typename Pool::second_type::value_type PagePtr;

		auto pool = &std::get<Pool>(m_pools);

		PagePtr page = nullptr;

		Handle* ptrCurFree = pool->first;
		Handle* ptrPrevFree = pool->first;

		std::div_t divToRemove = std::div(p_handle.index, OBJECT_POOL_PAGE_LENGTH);
		std::div_t divCurFree = std::div(ptrCurFree->index, OBJECT_POOL_PAGE_LENGTH);
		std::div_t divPrevFree = std::div(ptrCurFree->index, OBJECT_POOL_PAGE_LENGTH);

		while (divToRemove.quot < divCurFree.quot && divToRemove.rem < divCurFree.rem)
		{
			for (auto &p : pool->second)
			{
				if (!divCurFree.quot)
				{
					page = p;
					ptrPrevFree = ptrCurFree;
					ptrCurFree = reinterpret_cast<Handle*>(&page->data()[divCurFree.rem*p_handle.size]);

					divPrevFree = std::div(ptrPrevFree->index, OBJECT_POOL_PAGE_LENGTH);
					divCurFree = std::div(ptrCurFree->index, OBJECT_POOL_PAGE_LENGTH);
				}
				divCurFree.quot--;
			}
		}
	}
}
