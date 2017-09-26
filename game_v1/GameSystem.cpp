#include "GameSystem.hpp"

using namespace razaron::game;
using namespace razaron::physics;

/*constexpr ComponentArgs trasformComponentArgs(glm::vec2 t, glm::vec2 s, float r)
{
	return ComponentArgs{
		ComponentType::TRANSFORM,
		std::make_shared<TransformArgsTuple>(t, s, r)
	};
}*/

GameSystem::GameSystem()
{
    _componentTypes.insert(COMPONENT_ANIMAL);

    registerHandler(EVENT_MODEL, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_MODEL>(e.data);

        _models[e.recipient] = data->model;
    });

    registerHandler(EVENT_INPUT, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_INPUT>(e.data);

        for (auto &i : data->inputs)
        {
            switch (i.type)
            {
            case InputType::KEY_W:
            {
                g_cameraPos += glm::vec2{ 0.f, -0.1f };
                break;
            }

            case InputType::KEY_A:
            {
                g_cameraPos += glm::vec2{ -0.1f, 0.f };
                break;
            }

            case InputType::KEY_S:
            {
                g_cameraPos += glm::vec2{ 0.f, 0.1f };
                break;
            }

            case InputType::KEY_D:
            {
                g_cameraPos += glm::vec2{ 0.1f, 0.f };
                break;
            }

            default:
                break;
            }
        }
    });

    initGame();
}

GameSystem::~GameSystem()
{
}

Task GameSystem::update(EntityMap &entities, double delta)
{
    UNUSED(delta);

    // Get a vector of all the animals
    std::vector<std::pair<unsigned, AnimalComponent *>> animals;
    for (auto & [ id, components ] : entities)
    {
        try
        {
            auto animal = getObject<AnimalComponent>(components[COMPONENT_ANIMAL]);
            animals.emplace_back(id, animal);
        }
        catch (const std::exception &e)
        {
        }
    }

    // Seperate the animals into carnivores and herbivores
    std::vector<std::pair<unsigned, AnimalComponent *>> carnivores;
    std::vector<std::pair<unsigned, AnimalComponent *>> herbivores;
    for (auto &a : animals)
    {
        if (a.second->getDiet() == AnimalDiet::CARNIVORE)
            carnivores.push_back(a);
        else if (a.second->getDiet() == AnimalDiet::HERBIVORE)
            herbivores.push_back(a);
    }

    // Create steering events for closest carnivore-herbivore pairs
    std::vector<Event> events;
    for (auto &carnivore : carnivores)
    {
        if (_models.find(carnivore.first) == _models.end())
            continue;

        std::pair<unsigned, AnimalComponent *> prey;
        float distance = std::numeric_limits<float>::max();

        // Find closest prey
        for (auto &herbivore : herbivores)
        {
            if (_models.find(herbivore.first) == _models.end())
                continue;

            glm::vec2 p1 = glm::vec2((_models[carnivore.first])[3]);
            glm::vec2 p2 = glm::vec2((_models[herbivore.first])[3]);

            if (glm::length(p2 - p1) < distance)
            {
                prey = herbivore;
                distance = glm::length(p2 - p1) < distance;
            }
        }

        Event e{
            carnivore.first,
            EVENT_STEERING,
            std::make_shared<EVENTDATA_STEERING>(SteeringBehaviour::SEEK, prey.first)
        };

        events.push_back(e);
    }

    pushEvents(events);

    return Task{};
}

ComponentHandle GameSystem::createComponent(ComponentType type, std::shared_ptr<void> tuplePtr)
{
    UNUSED(tuplePtr);

    Handle h;

    switch (type)
    {
    case COMPONENT_ANIMAL:
    {
        AnimalArgs args = *(std::static_pointer_cast<AnimalArgs>(tuplePtr));

        h = emplaceObject<AnimalComponent>(std::get<0>(args));
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

bool GameSystem::removeComponent(ComponentHandle ch)
{
    Handle h;

    switch (ch.first)
    {
    default:
        return false;
        break;
    }

    return true;
}

void GameSystem::initGame()
{
    // Create new entity
    std::vector<Event> events;

    for (auto i = 0; i < 2; i++)
    {
        std::list<ComponentArgs> list;

        list.push_back(ComponentArgs{
            ComponentType::TRANSFORM,
            std::make_shared<TransformArgs>(
                glm::vec2{ i * 5.f, i * 5.f }, // translation
                glm::vec2{ 0.25f, 0.25f },     // scale
                0.f                            // rotation (radians)
                ) });

        list.push_back(ComponentArgs{
            ComponentType::MOTION,
            std::make_shared<MotionArgs>(
                5.f, // maxVelocity
                1.f, // maxAcceleration
                1.f  // mass
                ) });

        list.push_back(ComponentArgs{
            COMPONENT_ANIMAL,
            std::make_shared<AnimalArgs>(
                (i % 2) ? AnimalDiet::CARNIVORE : AnimalDiet::HERBIVORE) });

        Event a{
            0u,
            EventType::CREATE_ENTITY,
            std::make_shared<razaron::eventdata::CREATE_ENTITY>(list)
        };

        events.push_back(a);
    }

    pushEvents(events);
}
