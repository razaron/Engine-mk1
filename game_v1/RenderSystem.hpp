#pragma once

#include <map>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "System.hpp"
#include "ShapeComponent.hpp"
#include "config.hpp"

namespace razaron::render
{
    using namespace razaron::core::system;
    using namespace razaron::core::component;
    using namespace razaron::core::entity;
    using namespace razaron::graph;

    class RenderSystem : public System
    {
      public:
        RenderSystem();

        ~RenderSystem();

        Task update(EntityMap &entities, double delta);
        ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
        bool removeComponent(ComponentHandle ch);

        bool isOpen() { return _isOpen; }
        sf::Window *getWindow() { return &_window; }

      private:
        sf::RenderWindow _window;
        bool _isOpen = true;
        std::map<unsigned short, glm::mat4> _models;
    };
}
