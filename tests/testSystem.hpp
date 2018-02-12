#pragma once

#include "System.hpp"

using namespace razaron::core::system;
using namespace razaron::core::component;
using namespace razaron::core::entity;
using namespace razaron::graph;

class FooSystem : public System
{
public:
	FooSystem() {}
	~FooSystem() {}

	Task update(EntityMap &, double)
	{
		auto events = popEvents();

		for (auto &e : events)
		{
			if (e.type == EventType::REMOVE_COMPONENT)
			{
				auto data = std::static_pointer_cast<int>(e.data);

				count += *data;
			}
		}

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
			pushEvent(Event{ UUID64{0}, EventType::REMOVE_COMPONENT, std::make_shared<int>(i) });
		}

		return Task{};
	}

	ComponentHandle createComponent(ComponentType, std::shared_ptr<void>) { return ComponentHandle{}; }
	bool removeComponent(ComponentHandle) { return false; }

	std::string name{ "BarSystem" };
};
