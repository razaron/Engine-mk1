#include "PhysicsSystem.hpp"

using namespace razaron::physics;

double g_delay{ 1 };

PhysicsSystem::PhysicsSystem()
{
    _componentTypes.insert(ComponentType::TRANSFORM);
    _componentTypes.insert(ComponentType::MOTION);

    registerHandler(EVENT_STEERING, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_STEERING>(e.data);

        _behaviours[e.recipient] = std::make_pair(data->targetID, data->behaviour);
    });
}

PhysicsSystem::~PhysicsSystem()
{
}

Task PhysicsSystem::update(EntityMap &entities, double delta)
{
    g_delay -= delta;
    float pi = glm::pi<float>();

    // Get a map of all the physical bodies
    std::map<unsigned, std::pair<TransformComponent *, MotionComponent *>> bodies;
    for (auto & [ id, components ] : entities)
    {
        try
        {
            auto t = getObject<TransformComponent>(components[ComponentType::TRANSFORM]);
            auto m = getObject<MotionComponent>(components[ComponentType::MOTION]);

            bodies[id] = std::make_pair(t, m);
        }
        catch (const std::exception &e)
        {
        }
    }

    for (auto & [ id, components ] : bodies)
    {
        auto[t, m] = components;

        if (_behaviours.size() && _behaviours.find(id) != _behaviours.end() && bodies.find(_behaviours[id].first) != bodies.end())
        {
            auto[otherT, otherM] = bodies[_behaviours[id].first];
            UNUSED(otherM);

            switch (_behaviours[id].second)
            {
                case SteeringBehaviour::SEEK:
                {
                    seek(static_cast<float>(delta), t, m, otherT);
                    break;
                }
                case SteeringBehaviour::FLEE:
                {
                    flee(static_cast<float>(delta), t, m, otherT);
                    break;
                }
                case SteeringBehaviour::STOP:
                {
                    m->applyForce(-m->getVelocity());
                    m->updateVelocity(delta);
                    break;
                }
            }

            if (m->getVelocity().x != 0.f && m->getVelocity().y != 0.f)
            {
                // calculate angle to face target
                glm::vec2 v1{ 0.f, 1.f };
                glm::vec2 v2 = glm::normalize(m->getVelocity());

                auto dot = glm::dot(v1, v2);
                float theta = std::acos(std::max(std::min(dot, 1.f), -1.f));

                t->setRotation((v1.x < v2.x) ? pi - theta : theta - pi);
            }
        }

        // Send model matrix to the RenderSystem
        pushEvent(Event{
            id,
            EVENT_MODEL,
            std::make_shared<EVENTDATA_MODEL>(t->getModel()),
            1U });
    }

    return Task{};
}

void PhysicsSystem::seek(float delta, TransformComponent *t, MotionComponent *m, TransformComponent *target)
{
    glm::vec2 desiredVelocity = glm::normalize(target->translation - t->translation) * m->getMaxVelocity();
    glm::vec2 steering = desiredVelocity - m->getVelocity();

    m->applyForce(steering);
    m->updateVelocity(delta);

    t->translation += m->getVelocity() * delta;
}

void PhysicsSystem::flee(float delta, TransformComponent *t, MotionComponent *m, TransformComponent *target)
{
    glm::vec2 desiredVelocity = glm::normalize(t->translation - target->translation) * m->getMaxVelocity();
    glm::vec2 steering = desiredVelocity - m->getVelocity();

    m->applyForce(steering);
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
        {
            removeObject<TransformComponent>(ch.second);
            break;
        }
        case ComponentType::MOTION:
        {
            removeObject<MotionComponent>(ch.second);
            break;
        }
        default:
        {
            return false;
            break;
        }
    }

    return true;
}
