#include "Planner.hpp"

using namespace razaron::planner;

ActionList Planner::plan(ConditionSet worldState, Action goal, ActionList availableActions)
{
	_worldState = worldState;

	ActionGraph g;
	g.addEdge(0, 0, 0);
	g[0].data = goal;
	g[0].data.name = "GOAL: " + goal.name;
	g[0].adjacencyList.clear();

	genTree(&g, 0, availableActions, worldState, goal.preconditions);
	_lastPlan = ActionGraph{ g }; // 

	auto it = g.data.plans.begin();

	auto buildList = [&it](ActionVertex &v, ActionGraph &g) {
		std::clog << v.id << ": " << v.data.name << std::endl;

		// Create a new ActionList
		if (it == g.data.plans.end())
		{
			g.data.plans.emplace_back();
			it = std::prev(g.data.plans.end());
		}

		// Push Vertex onto current ActionList, ignoring origin Vertex
		if (v.id)
			it->push_back(v.data);

		// if branch end Vertex is State::WHITE, clear the ActionLists content to start filling again
		if (!v.adjacencyList.size())
		{
			std::clog << "END" << std::endl;

			g.data.plans.erase(it);
			it = g.data.plans.end();
		}
	};

	auto endList = [&it](ActionVertex &v, ActionGraph &g) {
		std::clog << v.id << ": " << v.data.name << std::endl;

		// Create a new ActionList
		if (it == g.data.plans.end())
		{
			g.data.plans.emplace_back();
			it = std::prev(g.data.plans.end());
		}

		// Push Vertex onto current ActionList, ignoring origin Vertex
		if (v.id)
			it->push_back(v.data);

		// if branch end Vertex is State::GREEN, go onto the next ActionList
		if (!v.adjacencyList.size())
		{
			std::clog << "END" << std::endl;

			it++;
		}
	};

	g.vertexFuncs[State::WHITE] = buildList;
	g.vertexFuncs[State::BLACK] = buildList;
	g.vertexFuncs[State::GREEN] = endList;

	g.branchTraversal(0);

	ActionList bestPlan;
	int bestCost = std::numeric_limits<int>::max();

	for (auto &plan : g.data.plans)
	{
		int sumCost = 0;
		for (auto &a : plan)
		{
			sumCost += a.cost;
		}

		if (sumCost < bestCost)
		{
			bestPlan = plan;
			bestCost = sumCost;
		}
	}

	if (bestPlan.size())
		bestPlan.reverse();

	return bestPlan;
}

void razaron::planner::Planner::toDOT(std::string filename)
{
	_lastPlan.vertexFuncs[State::WHITE] = [](ActionVertex &v, ActionGraph &g) {
		int cost = v.data.cost;

		for (auto &e : v.adjacencyList)
		{
			g[e.target].data.cost += cost;
		}

		if (!v.adjacencyList.size() && v.state != State::GREEN)
			v.state = State::RED;
	};

	_lastPlan.breadthFirstTraversal(0);

	auto vAttr = [](const ActionVertex &v) {
		auto id = v.id;
		auto name = v.data.name;
		auto cost = v.data.cost;

		auto colour = "white";
		switch (v.state)
		{
		case State::GREEN:
		{
			colour = "green";
			break;
		}
		case State::GREY:
		{
			colour = "grey";
			break;
		}
		case State::RED:
		{
			colour = "red";
			break;
		}
		}

		std::stringstream attributes;
		attributes << std::to_string(id) << "[label = \"" << name << "\" style=filled fillcolor=" << colour << "];\n";

		return attributes.str();
	};

	auto eAttr = [&](const ActionEdge &e) {
		std::stringstream attr;

		auto action = _lastPlan[e.target].data;

		attr << "[dir=back label=\"F(" << action.cost + e.data << ") G(" << action.cost << ") H(" << e.data << ")\"]\n";

		return attr.str();
	};

	std::ofstream dotFile(filename);
	auto dotStr = _lastPlan.getDOT(vAttr, eAttr);

	dotFile << dotStr;
}

void Planner::genTree(ActionGraph *g, int where, ActionList actions, ConditionSet currentState, ConditionSet goalState)
{
	// Checks the remaining conditions
	ConditionSet rem{ goalState };
	for (auto &c : goalState)
	{
		auto it = currentState.find(c);
		if (it != currentState.end())
		{
			if (it->value == c.value)
				rem.erase(c);
		}
	}

	if (!rem.size())
		return;

	for (auto &a : actions)
	{
		auto index = static_cast<unsigned short>(g->size());

		// Add postcondtions to the current state
		ConditionSet cState{ currentState };
		for (auto &c : a.postconditions)
		{
			auto it = goalState.find(c);
			if (it != goalState.end())
			{
				if (it->value == c.value)
				{
					cState.insert(c);
				}
			}
		}

		// add procedural postconditions to the current state
		bool relevantProc = false;
		for (auto &func : a.proceduralPostconditions)
		{
			auto prop = func(cState, goalState);
			cState.erase(prop.first);
			cState.insert(prop.first);

			// If the returned property is relevant to the goal
			if (prop.second)
				relevantProc = true;
		}

		// Checks the remaining conditions
		ConditionSet prevRem{ goalState };
		for (auto &c : goalState)
		{
			auto it = currentState.find(c);
			if (it != currentState.end())
			{
				if (it->value == c.value)
					prevRem.erase(c);
			}
		}
		
		ConditionSet curRem{ goalState };
		for (auto &c : goalState)
		{
			auto it = cState.find(c);
			if (it != cState.end())
			{
				if (it->value == c.value)
					curRem.erase(c);
			}
		}

		// True if there are remaining conditions
		bool x = curRem.size();
		
		// True if the remaining conditions has decreased or if a relevant 
		bool y = curRem.size() < prevRem.size() || relevantProc;
		
		// True if the action is applicable to the current world state
		bool z = includes(_worldState, a.preconditions);
		for (auto &func : a.proceduralPreconditions)
		{
			auto prop = func(_worldState, curRem);
			if (prop.first != WorldProperty{})
				z = false;
		}

		// If action satisfies some conditions OR if action satisfies all conditions but isn't applicable in current world state
		if ((x && y) || (!x && !z))
		{
			// Add action to graph
			g->addEdge(where, index, 0);
			(*g)[index].data = a;

			// Add preconditions to the goal state
			ConditionSet gState{ goalState };
			for (auto &c : a.preconditions)
			{
				// if precondition is in the current state, remove it
				auto it = cState.find(c);
				if (it != cState.end())
				{
					if (it->value == c.value)
						cState.erase(c);
				}
				// else add it to the goal state
				else
					gState.insert(c);
			}

			// Add procedural preconditions to the goal state
			for (auto &func : a.proceduralPreconditions)
			{
				// Add prop to goal state
				auto prop = func(cState, gState);

				if (prop.first != WorldProperty{})
				{
					gState.erase(prop.first);
					gState.insert(prop.first);
				}
			}

			int heuristic = 0;
			for (auto &c : gState)
			{
				auto it = cState.find(c);
				if (it != cState.end())
				{
					switch (c.value.index())
					{
					case 0:
					{
						if (std::get<bool>(c.value) != std::get<bool>(it->value))
							heuristic++;
						break;
					}
					case 1:
					{
						int current = std::get<int>(it->value);
						int goal = std::get<int>(c.value);

						heuristic += std::abs(current - goal);

						break;
					}
					}
				}
				else
					heuristic++;
			}

			for (auto &e : (*g)[where].adjacencyList)
			{
				if (e.target == index)
					e.data = heuristic;
			}

			// Remove the action from the list of available actions
			ActionList temp = actions;
			temp.remove_if([a](Action b) {
				return a.cost == b.cost && a.name == b.name;
			});

			// recursively build current branch if there are still untried actions
			if (temp.size())
				genTree(g, index, temp, cState, gState);
		}
		// If action satisfies all remaing conditions and is applicable in the current world state
		else if (!x && z)
		{
			g->addEdge(where, index, 0);
			(*g)[index].data = a;

			// if branch end satisfies goal, green light it
			(*g)[index].state = State::GREEN;
		}
	}
}