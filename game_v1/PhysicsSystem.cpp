#include "PhysicsSystem.hpp"

using namespace razaron::physics;

glm::vec2 g_target{ 0.5f, 0.5f };
double g_delay{ 1 };

PhysicsSystem::PhysicsSystem()
{
    _componentTypes.insert(ComponentType::TRANSFORM);
    _componentTypes.insert(ComponentType::MOTION);

    registerHandler(EVENT_INPUT, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_INPUT>(e.data);

        for (auto &i : data->inputs)
        {
            switch (i.type)
            {
            case InputType::MOUSE_LEFT:
            {
                g_target = glm::vec2{ i.rangeX, i.rangeY };
                g_target = g_target / g_cameraZoom + g_cameraPos;

                std::cout << "g_target: " << g_target.x << ", " << g_target.y << std::endl;
                std::cout << "g_cameraPos: " << g_cameraPos.x << ", " << g_cameraPos.y << std::endl;

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

Task PhysicsSystem::update(EntityMap &entities, double delta)
{
    g_delay -= delta;
    float pi = glm::pi<float>();
    for (auto & [ id, components ] : entities)
    {
        try
        {
            auto t = getObject<TransformComponent>(components[ComponentType::TRANSFORM]);
            auto m = getObject<MotionComponent>(components[ComponentType::MOTION]);

            seek(static_cast<float>(delta), t, m);

            // calculate angle to face target
            glm::vec2 v1{ 0.f, 1.f };
            glm::vec2 v2 = glm::normalize(m->getVelocity());
            float theta = glm::acos(glm::dot(v1, v2));

            t->setRotation((v1.x < v2.x) ? pi - theta : theta - pi);

            if (g_delay <= 0)
            {
                g_delay = 1;
                std::cout << "t->translation: " << t->translation.x << ", " << t->translation.y << std::endl;
                std::cout << "speed: " << glm::length(m->getVelocity()) << std::endl;
                std::cout << "acceleration: " << glm::length(m->getAcceleration()) << std::endl;
            }

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

void PhysicsSystem::seek(float delta, TransformComponent *t, MotionComponent *m)
{
    glm::vec2 desiredVelocity = glm::normalize(g_target - t->translation) * m->getMaxVelocity();
    glm::vec2 steering = desiredVelocity - m->getVelocity();

    m->applyForce(steering, delta);
    m->updateVelocity(delta);

    t->translation += m->getVelocity() * delta;
}

ComponentHandle PhysicsSystem::createComponent(ComponentType type, std::shared_ptr<void> tuplePtr)
{
    Handle h;

    switch (type)
    {
    case ComponentType::TRANSFORM:
    {
        TransformArgs args = *(std::static_pointer_cast<TransformArgs>(tuplePtr));

        h = emplaceObject<TransformComponent>(std::get<0>(args), std::get<1>(args), std::get<2>(args));
        break;
    }
    case ComponentType::MOTION:
    {
        MotionArgs args = *(std::static_pointer_cast<MotionArgs>(tuplePtr));

        h = emplaceObject<MotionComponent>(std::get<0>(args), std::get<1>(args), std::get<2>(args));
        break;
    }
    default:
    {
        h = Handle{};
        break;
    }
    }

    return ComponentHandle{ type, h };
}

bool PhysicsSystem::removeComponent(ComponentHandle ch)
{
    Handle h;

    switch (ch.first)
    {
    case ComponentType::TRANSFORM:
        removeObject<TransformComponent>(ch.second);
        break;
    default:
        return false;
        break;
    }

    return true;
}
