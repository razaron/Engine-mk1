#ifndef RZ_CORE_COMPONENT_HPP
#define RZ_CORE_COMPONENT_HPP

#include "Misc.hpp"

#include <atomic>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>

/*! Components carry the data for a specific feature. */
namespace rz::core
{
    //enum class ComponentType;
    struct ComponentType;
    struct ComponentTypeCmp;

    /*! Combines the size and position info of a Handle with the type information of a ComponentType. */
    using ComponentHandle = std::pair<ComponentType, Handle>;

    /*! A std::map type mapping Component Handle%s to their ComponentType%s. */
    using ComponentMap = std::map<ComponentType, Handle, ComponentTypeCmp>;

    using ComponentArgs = std::pair<ComponentType, std::shared_ptr<void>>;

    /*! Denotes the type of a derived Component object. */
    struct ComponentType
    {
        std::size_t hash;
        std::string debugName;

        ComponentType() = default;
        ComponentType(std::string name) : hash{ std::hash<std::string>()(name) }, debugName{ name } {}

        bool operator==(const ComponentType &rhs) const noexcept
        {
            return hash == rhs.hash;
        }

        bool operator!=(const ComponentType &rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator==(const std::string &name) const noexcept
        {
            return hash == std::hash<std::string>()(name);
        }

        bool operator!=(const std::string &name) const noexcept
        {
            return !(*this == name);
        }
    };

    struct ComponentTypeCmp
    {
        bool operator()(const ComponentType &lhs, const ComponentType &rhs) const
        {
            return lhs.hash < rhs.hash;
        }
    };

    /*! The base class for a Component. */
    class Component
    {
      public:
        Component() noexcept : _id{} {}              /*!< Default constructor. */
        Component(Component &&other) = default;      /*!< Move constructor. */
        Component(const Component &other) = default; /*!< Copy constructor. */
        ~Component() {}                              /*!< Default destructor. */

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
} // namespace rz::core

#endif // RZ_CORE_COMPONENT_HPP