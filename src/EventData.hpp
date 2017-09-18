#pragma once

#include <list>

#include "Component.hpp"

namespace razaron::eventdata
{
    using namespace razaron::core::component;

    struct CREATE_ENTITY
    {
        std::list<ComponentType> components;

        CREATE_ENTITY(std::list<ComponentType> p_components) : components{p_components} {}
    };

    struct CREATE_COMPONENT
    {
        ComponentType type;
        Handle handle;
        bool isCreated{false};

        CREATE_COMPONENT(ComponentType p_type) : type{p_type} {}
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
