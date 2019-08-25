#ifndef RZ_CORE_EVENTDATA_HPP
#define RZ_CORE_EVENTDATA_HPP

#include "Component.hpp"
#include "EventStream.hpp"

#include <list>
#include <memory>
#include <utility>

namespace rz::core::event::type
{
    const eventstream::EventType SPACE_NEW_ENTITY{"SPACE_NEW_ENTITY"};
    const eventstream::EventType SPACE_DELETE_ENTITY{"SPACE_DELETE_ENTITY"};
    const eventstream::EventType SPACE_REMOVE_ENTITY{"SPACE_REMOVE_ENTITY"};
    const eventstream::EventType SYSTEM_NEW_COMPONENT{"SYSTEM_NEW_COMPONENT"};
    const eventstream::EventType SYSTEM_DELETE_COMPONENT{"SYSTEM_DELETE_COMPONENT"};
    const eventstream::EventType ENTITY_ADD_COMPONENT{"ENTITY_ADD_COMPONENT"};
    const eventstream::EventType ENTITY_REMOVE_COMPONENT{"ENTITY_REMOVE_COMPONENT"};
}

namespace rz::core::event::data
{
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

        template <class... Args>
        SYSTEM_NEW_COMPONENT(ComponentType type, std::tuple<Args...> args) : type{ type }, argsPtr{ std::make_shared<std::tuple<Args...>>(args) } {}

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

#endif //RZ_CORE_EVENTDATA_HPP
