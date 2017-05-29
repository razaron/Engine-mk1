#include "ObjectPool.h"
#include "Component.h"
#include "EventStream.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "System.h"
#include "Space.h"
#include "Graph.h"

#include <iostream>
#include <chrono>
#include <functional>
#include <string>

AtomicCounter<unsigned> g_constructed;
AtomicCounter<unsigned> g_destroyed;
AtomicCounter<unsigned> g_foo;

using namespace razaron::core::space;
using namespace razaron::core::system;
using namespace razaron::graph;

class Foo8 : public Component
{
public:
	Foo8() {}
	Foo8(std::array<char, 4> p_data) :m_data(p_data) {}

	char* getData() { return m_data.data(); }
private:
	std::array<char, 4> m_data;
};

class Foo16 : public Component
{
public:
	Foo16() {}
	Foo16(std::array<char, 12> p_data) :m_data(p_data) {}

	char* getData() { return m_data.data(); }
private:
	std::array<char, 12> m_data;
};

class Foo32 : public Component
{
public:
	Foo32() {}
	Foo32(std::array<char, 28> p_data) :m_data(p_data) {}

	char* getData() { return m_data.data(); }
private:
	std::array<char, 28> m_data;
};

class Foo64 : public Component
{
public:
	Foo64() {}
	Foo64(std::array<char, 60> p_data) :m_data(p_data) {}

	char* getData() { return m_data.data(); }
private:
	std::array<char, 60> m_data;
};

class SystemA :public System
{
public:
	SystemA() {}
	~SystemA() {}

	Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
	Handle addComponent(Component* p_component) { return{}; }
	void removeComponent(Handle p_handle) {}

private:
	char* sysName{ "SystemA" };
};

class SystemC :public System
{
public:
	SystemC() {}
	~SystemC() {}

	Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
	Handle addComponent(Component* p_component) { return{}; }
	void removeComponent(Handle p_handle) {}

private:
	char* sysName{ "SystemC" };
};

class SystemD :public System
{
public:
	SystemD() {}
	~SystemD() {}

	Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
	Handle addComponent(Component* p_component) { return{}; }
	void removeComponent(Handle p_handle) {}

private:
	char* sysName{ "SystemD" };
};

class SystemB :public System
{
public:
	SystemB() {}
	~SystemB() {}

	Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
	Handle addComponent(Component* p_component) { return{}; }
	void removeComponent(Handle p_handle) {}

private:
	char* sysName{ "SystemB" };
};

class SystemE :public System
{
public:
	SystemE() {}
	~SystemE() {}

	Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
	Handle addComponent(Component* p_component) { return{}; }
	void removeComponent(Handle p_handle) {}

private:
	char* sysName{ "SystemE" };
};

class SystemF :public System
{
public:
	SystemF() {}
	~SystemF() {}

	Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
	Handle addComponent(Component* p_component) { return{}; }
	void removeComponent(Handle p_handle) {}

private:
	char* sysName{ "SystemF" };
};

void test(std::function<void()> testFunc, std::string testName, int runs, bool printCOut, bool printCLog)
{
	if (!printCLog)
		std::clog.setstate(std::ios_base::failbit);

	std::chrono::time_point<std::chrono::system_clock> start, end;

	std::cout << "Running " << testName << " test" << std::endl;

	double totalTime = 0;
	for (int i = 0; i < runs; i++)
	{
		start = std::chrono::system_clock::now();

		if (!printCOut)
			std::cout.setstate(std::ios_base::failbit);

		testFunc();

		if (!printCOut)
			std::cout.clear();

		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		//std::cout << std::endl;
		//std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;
		totalTime += elapsed_seconds.count();
	}

	std::cout << "Total time elapsed: " << totalTime << std::endl;
	std::cout << "Average time elapsed: " << totalTime / runs << std::endl;
	std::cout << "Average FPS: " << 1 / (totalTime / runs) << std::endl;
	std::cout << "constucted: " << g_constructed.get() << " destructed: " << g_destroyed.get() << std::endl;
	std::cout << std::endl;
	std::cout << "---------------------------------------------------------------------" << std::endl;

	g_constructed = 0;
	g_destroyed = 0;

	if (!printCLog)
		std::clog.clear();
}

void testObjectPool()
{
	ObjectPool pool;

	Handle h = pool.emplace<Foo64>(std::array<char, 60>{"foo"});
	h = pool.emplace<Foo64>(std::array<char, 60>{"bar"});
	Foo64* foo = pool.getObject<Foo64>(h);


	for (auto i = 0; i < 2047; i++)
	{
		//pool.emplace<Foo8>();
		//pool.emplace<Foo16>();
		//pool.emplace<Foo32>();
		pool.emplace<Foo64>();
	}
	std::cout << "sizeof Foo8: " << sizeof Foo8 << std::endl;

	std::cout << std::boolalpha;
	std::cout << "foo->data: " << foo->getData() << " foo->id: " << foo->getID() << std::endl;
	std::cout << "sizeof(*foo): " << sizeof(*foo) << " h.size: " << h.size << std::endl;
}

void testHeap()
{
	for (auto i = 0; i < 2047; i++)
	{
		//Foo8 *foo8 = new Foo8;
		//Foo16 *foo16 = new Foo16;
		//Foo32 *foo32 = new Foo32;
		Foo64 *foo64 = new Foo64;

		//delete foo8;
		//delete foo16;
		//delete foo32;
		delete foo64;
	}
}

void testEventStream()
{
	EventStream stream;

	//TEST SINGLE EVENT
	stream.pushEvent({ 1,EventType::TYPE_1,nullptr }, StreamType::INCOMING);
	stream.pushEvent({ 2,EventType::TYPE_2,nullptr }, StreamType::OUTGOING);

	std::cout << "TEST pushEvent/popEvent" << std::endl;
	std::cout << "outgoing recipient: " << stream.popEvent(StreamType::OUTGOING).recipient << std::endl;
	std::cout << "incoming recipient: " << stream.popEvent(StreamType::INCOMING).recipient << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	//TEST MULTIPLE EVENTS
	std::vector<Event> eIn;
	eIn.assign(1000, { 1,EventType::TYPE_1,nullptr });

	std::vector<Event> eOut;
	eOut.assign(1000, { 2,EventType::TYPE_2,nullptr });

	stream.pushEvents(eIn, StreamType::INCOMING);
	stream.pushEvents(eOut, StreamType::OUTGOING);

	eIn = stream.popEvents(StreamType::INCOMING);
	eOut = stream.popEvents(StreamType::OUTGOING);

	std::cout << "TEST pushEvent/popEvent" << std::endl;
	for (auto i = 0; i < 1000; i++)
	{
		std::cout << "outgoing recipient: " << eOut[i].recipient << " incoming recipient: " << eIn[i].recipient << std::endl;
	}
}

typedef Graph<char, char, char> TestGraph;
typedef Vertex<char, char> TestVertex;
typedef Edge<char> TestEdge;

TestGraph g;
unsigned short curID = 0;
void testGraph()
{
	g.onVertexDiscoverFunc = [](TestVertex& v, TestGraph& g) { g_foo++; };

	g.breadthFirstTraversal(0);

	std::cout << "g_foo == " << g_foo.get() << ", curID == " << curID << std::endl;
	g_foo = 0;
	g.reset();
}

using namespace razaron::render::component;
using namespace razaron::physics::component;
using namespace razaron::core::entity;

void testSpace()
{
	SystemA a;
	SystemB b;
	SystemC c;
	SystemD d;
	razaron::physics::PhysicsSystem e;
	razaron::render::RenderSystem f;

	enum V: unsigned short { A, B, C, D, E, F, G, H };

	SystemGraph g;
	g.addEdge('a', V::A, V::B);
	g.addEdge('a', V::A, V::C);
	g.addEdge('a', V::B, V::D);
	g.addEdge('a', V::C, V::E);
	g.addEdge('a', V::D, V::F);
	g.addEdge('a', V::E, V::F);

	g[V::A].data = &a;
	g[V::B].data = &b;
	g[V::C].data = &c;
	g[V::D].data = &d;
	g[V::E].data = &e;
	g[V::F].data = &f;

	Space s(std::move(g));
	Entity entity;

	//create render component
	Handle h = f.constructComponent<RenderComponent>();
	entity.addComponent({ ComponentType::RENDER, h });

	glm::mat4 modelMatrix = glm::mat4();

	std::array<glm::vec3, 3> vertices;
	vertices[0] = { -1.0f, -1.0f, 0.0f };
	vertices[1] = { 0.0f, 1.0f, 0.0f };
	vertices[2] = { 1.0f, -1.0f, 0.0f };

	std::array<unsigned short, 3> indices = { 2,1,0 };

	RenderComponent* r = f.getComponent<RenderComponent>(h);
	r->m_vertices = vertices;
	r->m_indices = indices;
	r->m_modelMatrix = modelMatrix;
	r->setOvColour({ 0.f,1.f,0.f });

	//create transform component
	h = e.constructComponent<TransformComponent>(glm::vec3{ 0.f,0.f,0.f }, glm::vec3{ 0.1f,0.1f, 0.1f });
	entity.addComponent({ ComponentType::TRANSFORM, h });

	s.addEntity(std::move(entity));

	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;

	for (auto i = 0; i < 100; i++) {
		start = std::chrono::system_clock::now();

		s.update(elapsed_seconds.count());
		end = std::chrono::system_clock::now();
		elapsed_seconds = end - start;
	}
}

int main() {

	std::vector<unsigned short> curRow;
	std::vector<unsigned short> nextRow;

	curRow.push_back(curID++);
	for (auto i = 0; i < 4; i++)
	{
		for (auto vertID : curRow)
		{
			for (auto j = 0; j < 8; j++)
			{
				g.addEdge('a', vertID, curID);
				nextRow.push_back(curID);
				curID++;
			}
		}

		curRow = nextRow;
		nextRow.clear();
	}

	test(testGraph, "Graph", 10, true, false);

	std::cin.get();
}