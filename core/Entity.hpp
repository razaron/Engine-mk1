#ifndef RZ_CORE_ENTITY_HPP
#define RZ_CORE_ENTITY_HPP

#include "Component.hpp"

#include <string>

/*!	Entities are a way of organizing related components into groups. */
namespace rz::core
{
    class Entity;

    using EntityMap = std::map<UUID64, Entity, UUID64Cmp>;

    /*!	The Entity class used for storing and getting Component Handle%s. 
    *   For more information and examples, see page \ref core.
	*/
    class Entity
    {
      public:
        Entity() noexcept : _id{0}, _components{} {} /*!< Default constructor. */

        /*!	Constructs an Entity object from a pre-constructed map of Component objects.
		*
		*	@param	components	The ComponentMap of components to initialize this Entity with.
		*/
        Entity(ComponentMap components) : _id{}, _components{ components } {}

        /*!	Gets the Handle mapped to the passed ComponentType.
		*
		*	@param		type					The ComponentType to search for.
		*
		*   @exception  std::invalid_argument   Throws if the ComponentType maps to no Handle.
		*
		*	@returns	A reference to the mapped Handle.
		*/
        // TODO const Handle &operator[](std::string typeName) const
        const Handle &operator[](ComponentType type) const
        {
            auto it = _components.find(type);
            if (it != _components.end())
                return it->second;
            else
                throw std::invalid_argument("ComponentType: "+ type.debugName +" not found in Entity: " + std::to_string(_id.uuid));
        }

        bool operator==(const Entity &rhs) const noexcept
        {
            return _id == rhs._id && _components == rhs._components;
        }

        bool operator!=(const Entity &rhs) const noexcept
        {
            return !(*this == rhs);
        }

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
            if (_components.find(component.first) != _components.end() && _components[component.first] == component.second)
                _components.erase(component.first);
        }

        template <typename... Types>
        bool has(Types... types) const
        {
            return (_components.count(types) && ...);
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
        const UUID64 &getID() const noexcept { return _id; }

      private:
        UUID64 _id;
        ComponentMap _components;
    };
}

#endif //RZ_CORE_ENTITY_HPP
