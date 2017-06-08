#pragma once

#include "Component.hpp"

/*!	Entities are a way of organizing related components into groups. */
namespace razaron::core::entity
{
	using namespace razaron::core::component;

	class Entity;

	using EntityMap = std::map<unsigned short, Entity>;

	/*!	The Entity class used for storing and getting Component Handle%s. */
	class Entity {
	public:
		Entity() /*! Default constructor. */
			:m_id{g_nextID++}, m_components{} {}

		/*!	Constructs an Entity object from a pre-constructed map of Component objects.
		*
		*	@param	p_components	The ComponentMap of components to initialize this Entity with.
		*/
		Entity(ComponentMap p_components)
			:m_id(g_nextID++), m_components(p_components) {}

		~Entity() {} /*! Default destructor */

		/*!	Adds a ComponentHandle to the ComponentMap of Entity.
		*
		*	@param	p_component		The ComponentHandle to add.
		*
		*	@retval	ComponentHandle	The passed ComponentHandle on success.
		*/
		ComponentHandle addComponent(ComponentHandle p_component)
		{
			m_components[p_component.first] = p_component.second;
			return p_component;
		}

		/*!	Removes a ComponentHandle from the ComponentMap of Entity.
		*
		*	@param	p_component		The ComponentHandle to remove.
		*/
		void removeComponent(ComponentHandle p_component)
		{
			if(m_components[p_component.first] == p_component.second)
				m_components.erase(p_component.first);
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

		/*!	Gets the Handle mapped to the passed ComponentType.
		*
		*	@param	p_type	The ComponentType to search for.
		*
		*	@retval	Handle	On success, the matching ComponentHandle.
		*	@retval	Handle	On failure, an empty Handle object.
		*/
		Handle operator[](ComponentType p_type)
		{
			if(m_components.find(p_type) != m_components.end())
				return m_components[p_type];
			else
				return Handle{};
		}

	private:
		unsigned m_id;
		ComponentMap m_components;
	};
}
