#pragma once

#include "Misc.hpp"

#include <atomic>
#include <iostream>
#include <limits>
#include <map>

/*! Components carry the data for a specific feature. */
namespace razaron::core::component
{
	enum class ComponentType;

	/*! Combines the size and position info of a Handle with the type information of a ComponentType. */
	using ComponentHandle = std::pair<ComponentType, Handle>;

	/*! A std::map type mapping Component Handle%s to their ComponentType%s. */
	using ComponentMap = std::map<ComponentType, Handle>;

	/*! Denotes the type of a derived Component object. */
	enum class ComponentType
	{
		DEFAULT,
		TRANSFORM,
		MOTION,
		SHAPE,
		COLLIDER,
		COMPONENT_1,
		COMPONENT_2,
		COMPONENT_3,
		COMPONENT_4,
		ENUM_SIZE
	};

	/*! The base class for a Component. */
	class Component
	{
	public:
		Component() noexcept : _id{} {}						/*!< Default constructor. */
		Component(Component &&other) = default;		/*!< Move constructor. */
		Component(const Component &other) = default;/*!< Copy constructor. */
		~Component() {}								/*!< Default destructor. */

		/*! Copy assignment operator overload that copies the ID of rhs into this Component. */
		Component &operator=(const Component &rhs) noexcept
		{
			_id = rhs._id;

			return *this;
		}

		/*! Move assignment operator overload that moves the ID of rhs into this Component. */
		Component &operator=(Component &&rhs) noexcept
		{
			if (this != &rhs)
			{
				_id = rhs._id;

				rhs._id.uuid = 0;
			}

			return *this;
		}

		/*! Gets the unique ID of this Component.
		*
		*	@returns The unique id of this Component.
		*/
		UUID64 getID() noexcept { return _id; }

	private:
		UUID64 _id;
	};
}