#include "Planner.hpp"

using namespace razaron::planner;

ActionList Planner::plan(Action goal, ActionList availableActions)
{
	ActionGraph g;

	g.addEdge(0,0,0);
	g[0].data = Action();

	genTree(&g, 0, goal, availableActions);

	auto it = g.data.plans.begin();
	
	g.vertexFuncs[State::WHITE] = [&it](ActionVertex &v, ActionGraph &g) {
		std::clog << v.id << ": " << v.data.name << std::endl;

		// Create a new ActionList
		if (it == g.data.plans.end())
		{
			g.data.plans.emplace_back();
			it = std::prev(g.data.plans.end());
		}

		// Push Vertex onto current ActionList, ignoring origin Vertex
		if(v.id)
			it->push_back(v.data);

		// if branch end Vertex is State::WHITE, clear the ActionLists content to start filling again
		if (!v.adjacencyList.size())
		{
			std::clog << "END" << std::endl;

			it->clear();
		}
	};

	g.vertexFuncs[State::GREEN] = [&it](ActionVertex &v, ActionGraph &g) {
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

	g.edgeFuncs[State::WHITE] = [](ActionEdge &e, ActionGraph &g) {
		std::clog << e.source << " --> " << e.target << std::endl;
	};

	g.depthFirstTraversal(0);
	
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

	if(bestPlan.size())
		bestPlan.reverse();

	return bestPlan;
}

void Planner::genTree(ActionGraph *g, int where, Action goal, ActionList actions)
{
	static ConditionSet tempWorldState{ _worldState };

	for (auto &a : actions)
	{
		auto index = static_cast<unsigned short>(g->size());

		auto checkProcedural = [](ProceduralConditionList list) 
		{ 
			bool satisfied = true;
			for (auto f : list)
			{
				if (!f()) 
				{
					satisfied = false;
					break;
				}
			}

			return satisfied;
		};


		// compare conditions. World -> Action -> Goal
		bool wToAStatic, wToADynamic, aToGStatic, aToGDynamic;

		// Compare the worlds current state against the Actions preconditions
		wToAStatic = includes(_worldState, a.preconditions);
		wToADynamic = checkProcedural(a.proceduralPreconditions);

		// Compare the actions postconditions against the goals preconditions
		aToGStatic = includes(a.postconditions, goal.preconditions);

		checkProcedural(a.proceduralPostconditions); // post conditions affect the blackboard
		aToGDynamic = checkProcedural(goal.proceduralPreconditions); // compares blackboard against real current values

		// CHECK IF BRANCH ENDS
		if (wToAStatic && wToADynamic && aToGStatic && aToGDynamic)
		{
			g->addEdge(where, index, 0);
			(*g)[index].data = a;

			// if branch end satisfies goal, green light it
			(*g)[index].state = State::GREEN;
		}
		// CHECK IF BRANCH CONTINUES
		else if (aToGStatic && aToGDynamic)
		{
			g->addEdge(where, index, 0);
			(*g)[index].data = a;

			ActionList temp = actions;
			temp.remove_if([a](Action b) {
				return a.cost == b.cost && a.name == b.name;
			});

			// recursively build current branch
			genTree(g, index, a, temp);
		}

		// Reset blackboard
		_reset();
	}
}