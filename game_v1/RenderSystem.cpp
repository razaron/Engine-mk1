#include "RenderSystem.hpp"

using namespace razaron::render;

RenderSystem::RenderSystem()
    : m_window{ sf::VideoMode(800, 800), "Game_V1" }
{
    // Update or insert a new model matrix into the map of models
    registerHandler(EVENT_MODEL, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_MODEL>(e.data);

        m_models[e.recipient] = data->model;
    });

    registerHandler(EVENT_INPUT, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_INPUT>(e.data);

        for (auto &i : data->inputs)
        {
            if (i.type == InputType::KEY_ESCAPE)
            {
                m_window.close();
                m_isOpen = false;
            }
        }
    });
}

RenderSystem::~RenderSystem()
{
}

Task RenderSystem::update(EntityMap &p_entities, double delta)
{
    UNUSED(p_entities);
    UNUSED(delta);

    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
            m_isOpen = false;
        }
    }

    // Clear window to black
    m_window.clear(sf::Color::Black);

    glm::mat4 view = glm::scale(glm::vec3{ m_window.getSize().x, m_window.getSize().y, 0.f });

    for (auto & [ id, model ] : m_models)
    {
        UNUSED(id);

        auto modelView = view*model;

        // Basic triangle
        std::array<glm::vec4, 4> points{ { { cosf(-0.5 * glm::pi<float>()), sinf(-0.5 * glm::pi<float>()), 0.f, 1.f },
                                           { cosf(glm::pi<float>() / 6), sinf(glm::pi<float>() / 6), 0.f, 1.f },
                                           { cosf(glm::pi<float>() - glm::pi<float>() / 6), sinf(glm::pi<float>() - glm::pi<float>() / 6), 1.f, 1.f },
                                           { 0.f, 0.f, 0.f, 1.f } } };

        // Transform triangle using model matrix
        //std::for_each(points.begin(), points.end(), [model](glm::vec4 &n){ n = model * n; });
        for (auto &vec : points)
        {
            vec = modelView * vec;
        }

        // Triangle
        sf::ConvexShape convex;
        convex.setFillColor(sf::Color::Blue);
        convex.setPointCount(3);
        convex.setPoint(0, sf::Vector2f{ points[0].x, points[0].y });
        convex.setPoint(1, sf::Vector2f{ points[1].x, points[1].y });
        convex.setPoint(2, sf::Vector2f{ points[2].x, points[2].y });

        // Line showing triangles direction
        sf::VertexArray v(sf::Lines, 2);
        v[0].position = sf::Vector2f{ points[0].x, points[0].y };
        v[1].position = sf::Vector2f{ points[3].x, points[3].y };
        v[1].color = sf::Color::Black;

        m_window.draw(convex);
        m_window.draw(v);
    }

    m_window.display();

    return Task{};
}

ComponentHandle RenderSystem::createComponent(ComponentType p_type)
{
    UNUSED(p_type);

    return ComponentHandle{};
}

bool RenderSystem::removeComponent(ComponentHandle p_ch)
{
    UNUSED(p_ch);

    return false;
}
