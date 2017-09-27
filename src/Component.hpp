#pragma once

#include <atomic>
#include <iostream>
#include <limits>
#include <map>

#include "Misc.hpp"

/*! Components carry the data for a specific feature. */
namespace razaron::core::component
{
    /*! Denotes the type of a derived Component object. */
    enum class ComponentType
    {
        DEFAULT,
        TRANSFORM,
        MOTION,
        SHAPE,
        COMPONENT_1,
        COMPONENT_2,
        COMPONENT_3,
        COMPONENT_4,
        ENUM_SIZE
    };

    /*! A unique unsigned int representing the next available unique ID. */
    extern std::atomic<unsigned> g_nextID;

    /*! Combines the size and position info of a Handle with the type information of a ComponentType. */
    using ComponentHandle = std::pair<ComponentType, Handle>;

    /*! A std::map type mapping Component Handle%s to their ComponentType%s. */
    using ComponentMap = std::map<ComponentType, Handle>;

    /*! The base class for a Component. */
    class Component
    {
      public:
        Component();                       /*!< Default constructor. */
        Component(Component &&other);      /*!< Move constructor. */
        Component(const Component &other); /*!< Copy constructor. */
        ~Component();                      /*!< Default destructor. */

        Component &operator=(const Component &rhs); /*!< Copy assignment operator overload that copies the ID of rhs into this Component. */
        Component &operator=(Component &&rhs);      /*!< Move assignment operator overload that moves the ID of rhs into this Component. */

        /*! Gets the unique ID of this Component.
		*
		*	@returns The unique id of this Component.
		*/
        unsigned getID() { return _id; }

      private:
        unsigned _id;
    };
}
