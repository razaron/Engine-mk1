#ifndef RZ_CORE_LUA_API_HPP
#define RZ_CORE_LUA_API_HPP

#include "Component.hpp"
#include "Entity.hpp"
#include "Space.hpp"

#include "sol.hpp"

namespace rz::lua
{
    namespace component
    {
        void bind(sol::state_view lua)
        {
            using namespace rz::core;
            lua.new_usertype<Component>("Component",
                                        sol::constructors<Component()>(),
                                        "getID", &Component::getID);

            lua.new_usertype<ComponentType>("ComponentType",
                                            sol::constructors<ComponentType(std::string)>());
        }
    } // namespace component

    namespace entity
    {
        void bind(sol::state_view lua)
        {
            using namespace rz::core;
            lua.new_usertype<Entity>("Entity",
                                     sol::constructors<Entity()>(),
                                     "getID", &Entity::getID,
                                     "has", &Entity::has<ComponentType>,
                                     sol::meta_function::index, &Entity::operator[]);

            lua["EntityVector"] = [](){ return std::vector<Entity>{}; };
            lua["EntityMap"] = [](){ return EntityMap{}; };
        }
    } // namespace entity

    namespace space
    {
        void bind(sol::state_view lua)
        {
            using namespace rz::core;
            lua.new_usertype<Space>("Space",
                                    "getEntities", &Space::getEntities,
                                    "getID", &Space::getID);
        }
    } // namespace space
} // namespace rz::lua

#endif //RZ_CORE_LUA_API_HPP
