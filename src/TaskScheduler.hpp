#pragma once

#include <functional>
#include <vector>

#include "Graph.hpp"

namespace razaron::taskscheduler
{
	using namespace razaron::graph;

	struct Task;
	struct TaskGraphData;

	using TaskGraph = Graph<Task, char, TaskGraphData>;
	using TaskGraphVertex = Vertex<Task, char>;
	using TaskGraphEdge = Edge<char>;

    struct Task
    {
        unsigned int taskID;
        unsigned int parentID;
        unsigned int dependencyID;
        std::function<void(Task *)> work;
        unsigned short openWorkItems;
    };

	/*! The data to be held by the TaskGraph. */
	struct TaskGraphData
	{

	};
}
