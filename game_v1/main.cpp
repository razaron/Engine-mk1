#include <chrono>

#include <SFML/Window.hpp>

#include "Space.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "InputSystem.hpp"
#include "EventData.hpp"

using namespace razaron::core::space;
using namespace razaron::input;
using namespace razaron::physics;
using namespace razaron::render;

int main()
{
    // Create new systems on the heap
	auto p = new PhysicsSystem{};
	auto w = new RenderSystem{};
    auto i = new InputSystem{w->getWindow()};

    // Construct a Space from a SystemGraph
    SystemGraph g;
	g.addEdge(0, 1);
    g.addEdge(1, 2);
    g[0].data = i;
	g[1].data = p;
    g[2].data = w;

    Space s{g};

	// Create new entity
	std::vector<Event> events;

	for(auto i=0;i<1000;i++)
	{
		events.push_back(Event{
			0u,
			EventType::CREATE_ENTITY,
			std::make_shared<razaron::eventdata::CREATE_ENTITY>(std::list<ComponentType>{ComponentType::TRANSFORM})
		});
	}

	s.pushEvents(events, StreamType::INCOMING);

	// Main loop
    auto start = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> delta{0};
	std::chrono::duration<double> elapsed{0};
    unsigned frameCount{0};
    while (w->isOpen())
    {
        s.update(delta.count());

        auto end = std::chrono::high_resolution_clock::now();
		delta = end - start;
        elapsed += delta;
        start = end;

        frameCount++;
        if(elapsed.count() > 1.0)
        {
            std::clog << "FPS: " << frameCount / elapsed.count() << std::endl;
            elapsed = elapsed.zero();
            frameCount = 0;
        }
    }

    return 0;
}
