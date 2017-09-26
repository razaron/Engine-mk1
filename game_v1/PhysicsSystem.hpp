#pragma once

#include <glm/glm.hpp>

#include "System.hpp"
#include "TransformComponent.hpp"
#include "MotionComponent.hpp"
#include "config.hpp"

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

        Task update(EntityMap &entities, double delta);
        ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
        bool removeComponent(ComponentHandle ch);

        void seek(float delta, TransformComponent* t, MotionComponent* m);

      private:
        std::map<unsigned, std::pair<unsigned, SteeringBehaviour>> _behaviours;
    };
}
