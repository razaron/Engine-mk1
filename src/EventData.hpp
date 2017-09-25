#pragma once

#include <list>
#include <utility>
#include <memory>

#include "Component.hpp"

namespace razaron::eventdata
{
    using namespace razaron::core::component;

    using ComponentArgs = std::pair<ComponentType, std::shared_ptr<void>>;

    struct CREATE_ENTITY
    {
        std::list<ComponentArgs> components;

        CREATE_ENTITY(std::list<ComponentArgs> p_components) : components{p_components} {}
    };

    struct CREATE_COMPONENT
    {
        ComponentType type;
        Handle handle;
        std::shared_ptr<void> argsPtr;
        bool isCreated{false};

        template <ComponentType Type, class... Args>
        CREATE_COMPONENT(std::tuple<Args...> p_args) : type{Type}, argsPtr{std::make_shared<std::tuple<Args...>>(p_args)} {}

        CREATE_COMPONENT(ComponentArgs p_args) : type{p_args.first}, argsPtr{p_args.second} {}

        CREATE_COMPONENT(ComponentHandle p_ch, bool p_isCreated) : type{p_ch.first}, handle{p_ch.second}, isCreated{p_isCreated} {}
    };

    struct REMOVE_ENTITY
    {
        bool initial;

        REMOVE_ENTITY(bool p_initial = true) : initial{p_initial} {}
    };

    struct REMOVE_COMPONENT
    {
        ComponentHandle ch;
        bool isRemoved{false};

        REMOVE_COMPONENT(ComponentHandle p_ch) : ch{p_ch} {}
        REMOVE_COMPONENT(ComponentHandle p_ch, bool p_isRemoved) : ch{p_ch}, isRemoved{p_isRemoved} {}
        //REMOVE_COMPONENT(ComponentHandle p_ch, bool p_isRemoved = false) : ch{p_ch}, isRemoved{p_isRemoved} {}
    };
}
