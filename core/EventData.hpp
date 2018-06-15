#ifndef RZ_CORE_EVENTDATA_H
#define RZ_CORE_EVENTDATA_H

#include "Component.hpp"

#include <list>
#include <memory>
#include <utility>

namespace rz::core
{
    using ComponentArgs = std::pair<ComponentType, std::shared_ptr<void>>;

    struct SPACE_NEW_ENTITY
    {
        std::list<ComponentArgs> components;

        SPACE_NEW_ENTITY(std::list<ComponentArgs> components) : components{ components } {}
    };

    struct SPACE_DELETE_ENTITY
    {
    };

    struct SPACE_REMOVE_ENTITY
    {
    };

    struct SYSTEM_NEW_COMPONENT
    {
        ComponentType type;
        std::shared_ptr<void> argsPtr;

        template <ComponentType Type, class... Args>
        SYSTEM_NEW_COMPONENT(std::tuple<Args...> args) : type{ Type }, argsPtr{ std::make_shared<std::tuple<Args...>>(args) } {}

        SYSTEM_NEW_COMPONENT(ComponentArgs args) : type{ args.first }, argsPtr{ args.second } {}
    };

    struct SYSTEM_DELETE_COMPONENT
    {
        ComponentHandle ch;

        SYSTEM_DELETE_COMPONENT(ComponentHandle ch) : ch{ ch } {}
    };

    struct ENTITY_ADD_COMPONENT
    {
        ComponentHandle ch;

        ENTITY_ADD_COMPONENT(ComponentHandle ch) : ch{ ch } {}
    };

    struct ENTITY_REMOVE_COMPONENT
    {
        ComponentHandle ch;

        ENTITY_REMOVE_COMPONENT(ComponentHandle ch) : ch{ ch } {}
    };
}

#endif //RZ_CORE_EVENTDATA_H