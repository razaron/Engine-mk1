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
    // Create new entity
    std::vector<Event> events;


    for (auto i = 0; i < 1000; i++)
    {
        std::list<ComponentArgs> list;

        list.push_back(ComponentArgs{
            ComponentType::TRANSFORM,
            std::make_shared<TransformArgs>(
                glm::vec2{ 10.f/1000*i, 5.f },
                glm::vec2{ 0.25f, 0.25f },
                0.f) });

        list.push_back(ComponentArgs{
            ComponentType::MOTION,
            std::make_shared<MotionArgs>(
                10.f,
                1.f,
                1.f) });

        Event a{
            0u,
            EventType::CREATE_ENTITY,
            std::make_shared<razaron::eventdata::CREATE_ENTITY>(list)
        };

        events.push_back(a);
    }

    pushEvents(events);

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
}

GameSystem::~GameSystem()
{
}

Task GameSystem::update(EntityMap &entities, double delta)
{
    UNUSED(entities);
    UNUSED(delta);

    return Task{};
}

ComponentHandle GameSystem::createComponent(ComponentType type, std::shared_ptr<void> tuplePtr)
{
    UNUSED(tuplePtr);

    Handle h;

    switch (type)
    {
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
