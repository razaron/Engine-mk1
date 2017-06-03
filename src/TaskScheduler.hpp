#pragma once

#include <vector>
#include <functional>

#include "Graph.hpp"

namespace razaron::taskscheduler
{
	struct Task
	{
		unsigned int taskID;
		unsigned int parentID;
		unsigned int dependencyID;
		std::function<void(Task*)> work;
		unsigned short openWorkItems;
	};

	namespace TaskGraph
	{
		using TaskGraph = int;
	}
}
