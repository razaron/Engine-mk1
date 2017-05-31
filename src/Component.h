#pragma once

#include <map>

#include "Misc.h"

/*! Components carry the data for a specific feature. */
namespace razaron::core::component
{
	/*! Denotes the type of a derived Component object. */
	enum class ComponentType { DEFAULT, FOO, BAR, RENDER, MESH, TRANSFORM, LOCOMOTION, COLLIDER, CONTROLLER, STEERING, ENUM_SIZE };

	/*! A unique unsigned int representing the next available unique ID. */
	extern AtomicCounter<unsigned> g_nextID;

	/*! Combines the size and position info of a Handle with the type information of a ComponentType. */
	using ComponentHandle = std::pair<ComponentType, Handle>;

	/*! A std::map type mapping Component Handle%s to their ComponentType%s. */
	using ComponentMap = std::map<ComponentType, Handle>;

	/*! The base class for a Component. */
	class Component {
	public:
		Component();						/*!< Default constructor. */
		Component(Component&& other);		/*!< Move constructor. */
		Component(const Component& other);	/*!< Copy constructor. */
		~Component();						/*!< Default destructor. */

		Component& operator=(const Component& rhs);	/*!< Copy assignment operator overload that copies the ID of rhs into this Component. */
		Component& operator=(Component&& rhs);		/*!< Move assignment operator overload that moves the ID of rhs into this Component. */

		/*! Gets the unique ID of this Component.
		*
		*	@returns The unique id of this Component.
		*/
		unsigned getID() { return m_id; }

	private:
		unsigned m_id;
	};
}