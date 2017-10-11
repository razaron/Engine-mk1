#include <chrono>

#include <SFML/Window.hpp>

#include "Space.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "InputSystem.hpp"
#include "GameSystem.hpp"
#include "EventData.hpp"

using namespace razaron::core::space;
using namespace razaron::input;
using namespace razaron::physics;
using namespace razaron::game;
using namespace razaron::render;

float g_cameraZoom = 1.f/25;
glm::vec2 g_cameraPos{0.f, 0.f};

int main()
{
    // Create new systems on the heap
	auto r = new RenderSystem{};
	auto i = new InputSystem{r->getWindow()};
	auto g = new GameSystem{};
	auto p = new PhysicsSystem{};

    // Construct a Space from a SystemGraph
    SystemGraph graph;
	graph.addEdge(0, 1);
	graph.addEdge(1, 2);
    graph.addEdge(2, 3);
	graph[0].data = i;
    graph[1].data = g;
	graph[2].data = p;
    graph[3].data = r;

    Space space{graph};

	// Main loop
    auto start = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> delta{0};
	std::chrono::duration<double> elapsed{0};
    unsigned frameCount{0};
    while (r->isOpen())
    {
		space.update(delta.count());

        auto end = std::chrono::high_resolution_clock::now();
		delta = end - start;
        elapsed += delta;
        start = end;

        frameCount++;
        if(elapsed.count() > 1.0)
        {
            std::clog << "FPS: " << frameCount / elapsed.count() << "\tEntities: " << space.getEntities().size() <<std::endl;
            elapsed = elapsed.zero();
            frameCount = 0;
        }
    }

    return 0;
}
