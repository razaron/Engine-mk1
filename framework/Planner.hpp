#pragma once

#include "Graph.hpp"
#include "Misc.hpp"

#include <set>
#include <list>
#include <iostream>
#include <fstream>
#include <string>

namespace razaron::planner
{
	using namespace razaron::graph;

	struct Action;
	struct ActionGraphData;

	using ActionGraph = Graph<Action, int, ActionGraphData>;
	using ActionVertex = Vertex<Action, int>;
	using ActionEdge = Edge<int>;

	using ProceduralCondition = std::function<bool()>;
	using ConditionSet = std::set<const char*>;
	using ProceduralConditionList = std::list<ProceduralCondition>;
	using ActionList = std::list<Action>;
	
	struct Action
	{
		unsigned cost{ 0 };
		std::string name{ "DEFAULT" };

		ConditionSet preconditions;
		ProceduralConditionList proceduralPreconditions;

		ConditionSet postconditions;
		ProceduralConditionList proceduralPostconditions;

		std::function<void()> effect;

		Action() {}
		Action(unsigned cost, std::string name, ConditionSet preconditions, ConditionSet postconditions, std::function<void()> effect)
			: cost(cost), name(name), preconditions(preconditions), postconditions(postconditions), effect(effect) {}

		Action(unsigned cost, std::string name, ConditionSet preconditions, ProceduralConditionList proceduralPreconditions, ConditionSet postconditions, std::function<void()> effect)
			: cost(cost), name(name), preconditions(preconditions), proceduralPreconditions(proceduralPreconditions), postconditions(postconditions), effect(effect) {}

		Action(unsigned cost, std::string name, ConditionSet preconditions, ConditionSet postconditions, ProceduralConditionList proceduralPostconditions, std::function<void()> effect)
			: cost(cost), name(name), preconditions(preconditions), postconditions(postconditions), proceduralPostconditions(proceduralPostconditions), effect(effect) {}

		Action(unsigned cost, std::string name, ConditionSet preconditions, ProceduralConditionList proceduralPreconditions, ConditionSet postconditions, ProceduralConditionList proceduralPostconditions, std::function<void()> effect)
			: cost(cost), name(name), preconditions(preconditions), proceduralPreconditions(proceduralPreconditions), postconditions(postconditions), proceduralPostconditions(proceduralPostconditions), effect(effect) {}
	};

	struct ActionGraphData
	{
		std::vector<ActionList> plans;
	};

	class Planner
	{
	public:
		Planner() {}
		Planner(ConditionSet worldState, std::function<void()> reset) :_worldState(worldState), _reset(reset) {}
		~Planner() {}

		ActionList plan(Action goal, ActionList availableActions);

		void toDOT(std::string filename);

	private:
		ConditionSet _worldState;
		std::function<void()> _reset;
		ActionGraph _lastPlan;

		void genTree(ActionGraph *g, int where, Action goal, ActionList actions);
	};
}