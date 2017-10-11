#include "GameSystem.hpp"

using namespace razaron::game;
using namespace razaron::physics;
using namespace razaron::render;

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
        if(e.lifetime)
            return;

        auto data = std::static_pointer_cast<EVENTDATA_MODEL>(e.data);

        _models[e.recipient] = data->model;
    });

    registerHandler(EVENT_INPUT, [&](Event &e) {
        auto data = std::static_pointer_cast<EVENTDATA_INPUT>(e.data);

        for (auto &i : data->inputs)
        {
            switch (i.type)
            {
            case InputType::MOUSE_RIGHT:
            {
                auto target = glm::vec2{ i.rangeX, i.rangeY };
                target = target / g_cameraZoom + g_cameraPos;

                Event e = createAnimal(target, glm::vec2{ 0.25f, 0.25f }, AnimalDiet::HERBIVORE, glm::vec4{ 0.f, 1.f, 0.f, 1.f });

                pushEvent(e);

                break;
            }
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

    auto lambda = [&](Event & e) {
        auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

        if(data->ch.first == ComponentType::TRANSFORM)
        {
            _models.erase(e.recipient);
        }
    };

    _eventStream.extendHandler(EventType::REMOVE_COMPONENT, lambda);

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
        if (a.second->getDiet() == AnimalDiet::CARNIVORE && _models.find(a.first) != _models.end())
            carnivores.push_back(a);
        else if (a.second->getDiet() == AnimalDiet::HERBIVORE && _models.find(a.first) != _models.end())
            herbivores.push_back(a);
    }

    if(!carnivores.size())
        return Task{};

    // move camera to first carnivore
    g_cameraPos = glm::vec2((_models[(carnivores[0]).first])[3]) - glm::vec2{0.5f/g_cameraZoom, 0.5f/g_cameraZoom};

    if(!herbivores.size())
        return Task{};

    // Create SEEK events for closest carnivore-herbivore pairs
    updateSteering(herbivores, carnivores);

    return Task{};
}

void GameSystem::updateSteering(std::vector<Animal> herbivores, std::vector<Animal> carnivores)
{
    // Finds closest prey, and SEEKs it
    std::vector<Event> events;
    for (auto &carnivore : carnivores)
    {
        Animal prey;
        while((prey = findNearestAnimal(carnivore, herbivores)) != Animal{})
        {
            glm::vec2 p1 = glm::vec2((_models[prey.first])[3]);
            glm::vec2 p2 = glm::vec2((_models[carnivore.first])[3]);

            if (glm::length(p2 - p1) <= 1.f)
            {
                Event e{
                    prey.first,    // Entity ID.
                    EventType::REMOVE_ENTITY, // Event type enum
                    std::make_shared<eventdata::REMOVE_ENTITY>()
                };

                events.push_back(e);
                //std::remove(herbivores.begin(), herbivores.end(), prey);
                herbivores.erase(std::remove(herbivores.begin(), herbivores.end(), prey), herbivores.end());
            }
            else
            {
                Event e{
                    carnivore.first,
                    EVENT_STEERING,
                    std::make_shared<EVENTDATA_STEERING>(SteeringBehaviour::SEEK, prey.first)
                };

                events.push_back(e);

                break;
            }
        }

    }

    // Run from nearby predators or eat if safe
    for (auto &herbivore : herbivores)
    {
        // Find nearby predator predator
        Animal predator = findNearestAnimal(herbivore, carnivores, 5.f);

        if(predator != Animal{})
        {
            Event e{
                herbivore.first,
                EVENT_STEERING,
                std::make_shared<EVENTDATA_STEERING>(SteeringBehaviour::FLEE, predator.first)
            };

            events.push_back(e);
        }
        else
        {
            Event e{
                herbivore.first,
                EVENT_STEERING,
                std::make_shared<EVENTDATA_STEERING>(SteeringBehaviour::STOP, 0)
            };

            events.push_back(e);
        }
    }

    pushEvents(events);
}

Animal GameSystem::findNearestAnimal(const Animal &me, std::vector<Animal> others, float maxDist)
{
    Animal a{};
    float distance = maxDist;

    for (const auto &other : others)
    {
        glm::vec2 p1 = glm::vec2((_models[me.first])[3]);
        glm::vec2 p2 = glm::vec2((_models[other.first])[3]);

        if (glm::length(p2 - p1) < distance)
        {
            a = other;
            distance = glm::length(p2 - p1);
        }
    }

    return a;
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
    case COMPONENT_ANIMAL:
    {
        removeObject<AnimalComponent>(ch.second);
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

void GameSystem::initGame()
{
    // Create new entity
    std::vector<Event> events;

    for (auto i = 0; i < 1; i++)
    {
        auto pos = glm::vec2{ i * 1.f, i * 1.f };
        auto scale = glm::vec2{ 0.25f, 0.25f };
        auto colour = glm::vec4{ 1.f, 0.f, 0.f, 1.f };

        auto e = createAnimal(pos, scale, AnimalDiet::CARNIVORE, colour);

        events.push_back(e);
    }

    const float max = 100;
    for (auto x = 0.f; x < std::sqrt(max); x++)
    {
        for (auto y = 0.f; y < std::sqrt(max); y++)
        {
            //if(x == y)
            //    continue;

            auto pos = glm::vec2{ x * 10.f / std::sqrt(max), y * 10.f / std::sqrt(max) };
            auto scale = glm::vec2{ 0.25f, 0.25f };
            auto colour = glm::vec4{ 0.f, 1.f, 0.f, 1.f };

            auto e = createAnimal(pos, scale, AnimalDiet::HERBIVORE, colour);

            //events.push_back(e);
        }
    }

    pushEvents(events);
}

Event GameSystem::createAnimal(glm::vec2 pos, glm::vec2 scale, AnimalDiet diet, glm::vec4 colour)
{
    std::list<ComponentArgs> list;

    list.push_back(ComponentArgs{
        ComponentType::TRANSFORM,
        std::make_shared<TransformArgs>(
            glm::vec2{ pos.x, pos.y },      // translation
            glm::vec2{ scale.x, scale.y },  // scale
            0.f                             // rotation (radians)
        )
    });

    list.push_back(ComponentArgs{
        ComponentType::MOTION,
        std::make_shared<MotionArgs>(
            (diet == AnimalDiet::HERBIVORE) ? .5f : 5.f, // maxVelocity
            (diet == AnimalDiet::HERBIVORE) ? .1f : 100000.f,  // maxAcceleration
            1.f                                           // mass
        )
    });

    list.push_back(ComponentArgs{
        COMPONENT_ANIMAL,
        std::make_shared<AnimalArgs>(
            diet
        )
    });

    list.push_back(ComponentArgs{
        ComponentType::SHAPE,
        std::make_shared<ShapeArgs>(
            colour
        )
    });

    Event e{
        0u,
        EventType::CREATE_ENTITY,
        std::make_shared<razaron::eventdata::CREATE_ENTITY>(list)
    };

    return e;
}
