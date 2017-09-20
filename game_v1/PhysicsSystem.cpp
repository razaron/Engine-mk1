#include "PhysicsSystem.hpp"
#include "TransformComponent.hpp"
#include "config.hpp"

using namespace razaron::physics;

glm::vec2 g_position{ 0.f, 0.f }, g_target{ 0.5f, 0.5f }, g_velocity{ 0.f, 0.f };
double g_delay{ 0.5 };

PhysicsSystem::PhysicsSystem()
{
    m_componentTypes.insert(ComponentType::TRANSFORM);

    registerHandler(EVENT_INPUT, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_INPUT>(e.data);

        for (auto &i : data->inputs)
        {
            switch (i.type)
            {
            case InputType::MOUSE_LEFT:
            {
                g_target = { i.rangeX, i.rangeY };
                break;
            }

            case InputType::MOUSE_RIGHT:
            {
                g_position = { i.rangeX, i.rangeY };
                break;
            }

            case InputType::KEY_W:
            {
                g_position += glm::vec2{ 0.f, -0.01f };
                break;
            }

            case InputType::KEY_A:
            {
                g_position += glm::vec2{ -0.01f, 0.f };
                break;
            }

            case InputType::KEY_S:
            {
                g_position += glm::vec2{ 0.f, 0.01f };
                break;
            }

            case InputType::KEY_D:
            {
                g_position += glm::vec2{ 0.01f, 0.f };
                break;
            }

            default:
                break;
            }
        }
    });
}

PhysicsSystem::~PhysicsSystem()
{
}

Task PhysicsSystem::update(EntityMap &p_entities, double delta)
{
    g_delay -= delta;
    float pi = glm::pi<float>();
    for (auto & [ id, components ] : p_entities)
    {
        try
        {
            auto t = getObject<TransformComponent>(components[ComponentType::TRANSFORM]);

            auto u = glm::normalize(g_target - g_position) * 0.001f;

            // truncate steering
            auto steering = u - g_velocity;
            float maxForce = 0.000000001f;
            if (glm::length(steering) >= maxForce)
            {
                steering = glm::normalize(steering)*maxForce;
            }

            // truncate steering
            g_velocity += steering;
            float maxVelocity = 0.001f;
            if (glm::length(g_velocity) >= maxVelocity)
            {
                steering = glm::normalize(g_velocity)*maxVelocity;
            }

            g_position += g_velocity*static_cast<float>(delta);
            //g_position += u * static_cast<float>(delta);

            t->translation = g_position;

            // calculate angle to face target
            glm::vec2 v1{ 0.f, 1.f };
            glm::vec2 v2 = glm::normalize(g_target - g_position);
            float theta = glm::acos(glm::dot(v1, v2));

            t->setRotation((v1.x < v2.x) ? pi - theta : theta - pi);

            // Send model matrix to the RenderSystem
            pushEvent(Event{
                id,
                EVENT_MODEL,
                std::make_shared<EVENTDATA_MODEL>(t->getModel()) });
        }
        catch (const std::exception &e)
        {
        }
    }

    return Task{};
}

ComponentHandle PhysicsSystem::createComponent(ComponentType p_type)
{
    Handle h;

    switch (p_type)
    {
    case ComponentType::TRANSFORM:
        h = emplaceObject<TransformComponent>();
        break;
    default:
        h = Handle{};
        break;
    }

    return ComponentHandle{ p_type, h };
}

bool PhysicsSystem::removeComponent(ComponentHandle p_ch)
{
    Handle h;

    switch (p_ch.first)
    {
    case ComponentType::TRANSFORM:
        removeObject<TransformComponent>(p_ch.second);
        break;
    default:
        return false;
        break;
    }

    return true;
}
