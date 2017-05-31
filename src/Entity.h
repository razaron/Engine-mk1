#pragma once

#include <vector>

#include "Component.h"

/*!	Entities are a way of organizing related components into groups. */
namespace razaron::core::entity
{
	using namespace razaron::core::component;
	
	/*!	The Entity class used for storing and getting Component Handle%s. */
	class Entity {
	public:
		Entity() {} /*! Default constructor. */

		/*!	Constructs an Entity object from a pre-constructed map of Component objects.
		*
		*	@param	p_components	The ComponentMap of components to initialize this Entity with.
		*/
		Entity(ComponentMap p_components)
			:m_components(p_components), m_id(g_nextID++) {}

		~Entity() {} /*! Default destructor */

		/*!	Adds a ComponentHandle to the ComponentMap of Entity
		*
		*	@param	p_component		The ComponentHandle to add
		*
		*	@retval	ComponentHandle	The passed ComponentHandle on success
		*/
		ComponentHandle addComponent(ComponentHandle p_component)
		{
			m_components[p_component.first] = p_component.second;
			return p_component;
		}

		/*!	Gets the current ComponentMap of the Entity.
		*
		*	@returns	The current ComponentMap of the Entity
		*/
		ComponentMap getComponents()
		{ 
			return m_components; 
		};

		/*! Gets the unique ID of this Entity. 
		*
		*	@returns	The unique id of this Entity.
		*/
		unsigned int getID() { return m_id; }

		/*!	Gets the Handle mapped to the passed ComponentType
		*
		*	@param	p_type	The ComponentType to search for
		*
		*	@retval	Handle	The passed ComponentHandle on success
		*/
		Handle& operator[](ComponentType p_type)
		{
			return m_components[p_type];
		}

	private:
		unsigned int m_id;
		ComponentMap m_components;
	};
}