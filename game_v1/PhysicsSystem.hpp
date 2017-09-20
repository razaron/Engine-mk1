#pragma once

#include <glm/glm.hpp>

#include "System.hpp"

namespace razaron::physics
{
    using namespace razaron::core::system;
    using namespace razaron::core::component;
    using namespace razaron::core::entity;
    using namespace razaron::graph;

    class PhysicsSystem : public System
    {
      public:
        PhysicsSystem();

        ~PhysicsSystem();

        Task update(EntityMap &p_entities, double delta);
        ComponentHandle createComponent(ComponentType p_type);
        bool removeComponent(ComponentHandle p_ch);
      private:
    };
}
