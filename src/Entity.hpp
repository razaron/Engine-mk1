#pragma once

#include <string>

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
			:_id{g_nextID++}, _components{} {}

		/*!	Constructs an Entity object from a pre-constructed map of Component objects.
		*
		*	@param	components	The ComponentMap of components to initialize this Entity with.
		*/
		Entity(ComponentMap components)
			:_id(g_nextID++), _components(components) {}

		~Entity() {} /*! Default destructor */

		/*!	Adds a ComponentHandle to the ComponentMap of Entity.
		*
		*	@param	component		The ComponentHandle to add.
		*
		*	@retval	ComponentHandle	The passed ComponentHandle on success.
		*/
		ComponentHandle addComponent(ComponentHandle component)
		{
			_components[component.first] = component.second;
			return component;
		}

		/*!	Removes a ComponentHandle from the ComponentMap of Entity.
		*
		*	@param	component		The ComponentHandle to remove.
		*/
		void removeComponent(ComponentHandle component)
		{
			if(_components.find(component.first) != _components.end() && _components[component.first] == component.second)
				_components.erase(component.first);
		}

		/*!	Gets the current ComponentMap of the Entity.
		*
		*	@returns	The current ComponentMap of the Entity
		*/
		ComponentMap getComponents()
		{
			return _components;
		};

		/*! Gets the unique ID of this Entity.
		*
		*	@returns	The unique id of this Entity.
		*/
		unsigned int getID() { return _id; }

		/*!	Gets the Handle mapped to the passed ComponentType.
		*
		*	@param		type					The ComponentType to search for.
		*
        *   @exception  std::invalid_argument   Throws if the ComponentType maps to no Handle.
        *
		*	@returns	A reference to the mapped Handle.
		*/
		Handle &operator[](ComponentType type)
		{
			if(_components.find(type) != _components.end())
				return _components[type];
			else
				throw std::invalid_argument("ComponentType not found in Entity: " + std::to_string(_id));
		}

	private:
		unsigned _id;
		ComponentMap _components;
	};
}
