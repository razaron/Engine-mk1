#pragma once

#include <list>
#include <utility>
#include <memory>

#include "Component.hpp"

namespace razaron::core::eventdata
{
    using namespace razaron::core::component;

    using ComponentArgs = std::pair<ComponentType, std::shared_ptr<void>>;

    struct CREATE_ENTITY
    {
        std::list<ComponentArgs> components;

        CREATE_ENTITY(std::list<ComponentArgs> components) : components{components} {}
    };

    struct CREATE_COMPONENT
    {
        ComponentType type;
        Handle handle;
        std::shared_ptr<void> argsPtr;
        bool isCreated{false};

        template <ComponentType Type, class... Args>
        CREATE_COMPONENT(std::tuple<Args...> args) : type{Type}, argsPtr{std::make_shared<std::tuple<Args...>>(args)} {}

        CREATE_COMPONENT(ComponentArgs args) : type{args.first}, argsPtr{args.second} {}

        CREATE_COMPONENT(ComponentHandle ch, bool isCreated) : type{ch.first}, handle{ch.second}, isCreated{isCreated} {}
    };

    struct REMOVE_ENTITY
    {
        bool initial;

        REMOVE_ENTITY(bool initial = true) : initial{initial} {}
    };

    struct REMOVE_COMPONENT
    {
        ComponentHandle ch;
        bool isRemoved{false};

        REMOVE_COMPONENT(ComponentHandle ch) : ch{ch} {}
        REMOVE_COMPONENT(ComponentHandle ch, bool isRemoved) : ch{ch}, isRemoved{isRemoved} {}
        //REMOVE_COMPONENT(ComponentHandle ch, bool isRemoved = false) : ch{ch}, isRemoved{isRemoved} {}
    };
}
