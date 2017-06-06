#pragma once

#include "System.hpp"
#include "RenderComponent.hpp"

namespace razaron::render
{
	using namespace razaron::render::component;
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;
	using namespace razaron::graph;

	class RenderSystem :public System
	{
	public:
		RenderSystem() {}

		~RenderSystem()
		{
			Event e = m_eventStream.popEvent(StreamType::INCOMING);
			Event eNull = {};
			while (e != eNull)
			{
				if (e.type == EventType::TYPE_1)
				{
					glm::mat4* foo = static_cast<glm::mat4*>(e.data);
					delete foo;
				}

				e = m_eventStream.popEvent(StreamType::INCOMING);
			}
		}

		Graph<Task, char, TaskGraphData>* update(std::vector<Entity>* p_entities, double delta)
		{
			Event e = m_eventStream.popEvent(StreamType::INCOMING);
			Event eNull = {};
			while (e != eNull)
			{
				if (e.type == EventType::TYPE_1)
				{
					glm::mat4* foo = static_cast<glm::mat4*>(e.data);
					delete foo;
				}

				e = m_eventStream.popEvent(StreamType::INCOMING);
			}

			std::cout << m_name << std::endl;
			return nullptr;
		}

	private:
		std::string m_name{ "Render System" };
	};
}
