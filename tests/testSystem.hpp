#pragma once

#include "System.hpp"

using namespace razaron::core::system;
using namespace razaron::core::component;
using namespace razaron::core::entity;
using namespace razaron::graph;

// 
class FooSystem : public System
{
public:
	FooSystem() 
	{
		extendHandler(EventType::EVENT_1, [&](const Event& e) {
			auto data = std::static_pointer_cast<int>(e.data);

			count += *data;
			}
		);
	}
	
	~FooSystem() {}

	Task update(EntityMap &, double)
	{
		return Task{};
	}

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

	int count{};
	std::string name{ "FooSystem" };
};

class BarSystem : public System
{
public:
	BarSystem() {}
	~BarSystem() {}

	Task update(EntityMap &, double)
	{
		for (auto i = 0; i < 5; i++)
		{
			_eventStream.pushEvent(Event{ UUID64{0}, EventType::EVENT_1, std::make_shared<int>(i) }, StreamType::OUTGOING);
		}

		return Task{};
	}

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

	std::string name{ "BarSystem" };
};
