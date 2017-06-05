#include "PhysicsSystem.hpp"

using namespace razaron::physics;
using namespace razaron::physics::component;

Graph<Task, char, TaskGraphData>* PhysicsSystem::update(std::vector<Entity>* p_entities, double delta)
{
	std::map<unsigned int, Entity> entityMap;
	for (auto&& e : *p_entities)
	{
		entityMap[e.getID()] = e;
	}

	Event e = m_eventStream.popEvent(StreamType::INCOMING);
	Event eNull = {};
	while (e != eNull)
	{
		Entity entity = entityMap[e.recipient];
		auto r = m_pool.getObject<TransformComponent>(entity[ComponentType::TRANSFORM]);

		UNUSED(r);

		//DO STUFF

		e = m_eventStream.popEvent(StreamType::INCOMING);
	}

	for (auto& e : entityMap)
	{
		auto t = m_pool.getObject<TransformComponent>(e.second[ComponentType::TRANSFORM]);
		float angle = glm::radians(static_cast<float>(36.0*delta));
		glm::quat q = glm::angleAxis(angle, glm::vec3(0.f, 0.f, 1.f));
		t.rotate(q);

		glm::mat4 *model = new glm::mat4(t.getModel());


		Event ev = {};
		ev.recipient = e.second.getID();
		ev.type = EventType::TYPE_1;
		ev.data = model;

		m_eventStream.pushEvent(ev, StreamType::OUTGOING);
	}

	return nullptr;
}
