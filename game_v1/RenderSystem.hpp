#pragma once

#include <map>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "System.hpp"
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

        Task update(EntityMap &p_entities, double delta);
        ComponentHandle createComponent(ComponentType p_type);
        bool removeComponent(ComponentHandle p_ch);

        bool isOpen() { return m_isOpen; }
        sf::Window *getWindow() { return &m_window; }

      private:
        sf::RenderWindow m_window;
        bool m_isOpen = true;
        std::map<unsigned short, glm::mat4> m_models;
    };
}
