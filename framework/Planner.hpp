#pragma once

#include "Graph.hpp"
#include "Misc.hpp"

#include <unordered_set>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <variant>

namespace razaron::planner
{
	using namespace razaron::graph;

	using PropertyVariant = std::variant<bool, int>;

	/*! Represents a facet of the world as a property interpretable by the planner. */
	struct WorldProperty
	{
		std::size_t id;			/*!< A hashed `std::string` denoting the property owner. */
		std::size_t type;		/*!< A hashed `std::string` denoting the property type. */
		PropertyVariant value;	/*!< A `std::variant` denoting the value off the property. */
		std::string debugID;
		std::string debugType;

		WorldProperty() :id(0), type(0), value(false) {}
		WorldProperty(std::string id, std::string type, PropertyVariant value)
			:id(std::hash<std::string>{}(id)), type(std::hash<std::string>{}(type)), value(value), debugID(id), debugType(type) {}

		bool operator==(const WorldProperty& rhs)
		{
			return id == rhs.id && type == rhs.type && value == rhs.value;
		}

		bool operator!=(const WorldProperty& rhs)
		{
			return !(*this == rhs);
		}
	};

	struct WorldPropertyHash
	{
		std::size_t operator()(const WorldProperty& p) const
		{
			return p.id ^ (p.type << 1);
		}
	};

	struct WorldPropertyEqual
	{
		bool operator()(const WorldProperty& lhs, const WorldProperty& rhs) const
		{
			return lhs.id == rhs.id && lhs.type == rhs.type;
		}
	};

	struct Action;
	struct ActionGraphData;

	using ActionGraph = Graph<Action, int, ActionGraphData>;
	using ActionVertex = Vertex<Action, int>;
	using ActionEdge = Edge<int>;

	using ActionList = std::list<Action>;
	using ConditionSet = std::unordered_set<WorldProperty, WorldPropertyHash, WorldPropertyEqual>;
	using ProceduralCondition = std::function<std::pair<WorldProperty, bool>(const ConditionSet&, const ConditionSet&)>;
	using ProceduralConditionList = std::list<ProceduralCondition>;

	/*! A single atomic Action used to modify the world state in some manner. */
	struct Action
	{
		unsigned cost{ 0 };									/*!< The cost to perform this Action. */
		std::string name{ "DEFAULT" };						/*!< The Action%s name. */

		ConditionSet preconditions;							/*!< Are added to the goal world state in a binary manner, replacing any WorldProperty with a matching id and type. */
		ProceduralConditionList proceduralPreconditions;	/*!< Functors that take the current and goal world states to return a WorldProperty to add as another precondition. */

		ConditionSet postconditions;						/*!< Are added to the current world state in a binary manner, replacing any WorldProperty with a matching id and type. */
		ProceduralConditionList proceduralPostconditions;	/*!< Functors that take the current and goal world states to return a WorldProperty to add as another postcondition. */

		std::function<void()> effect;						/*!< A function the runs the logic for this Action. Not used during planning. */

		Action() {}
		Action(unsigned cost, std::string name, ConditionSet preconditions, ConditionSet postconditions, std::function<void()> effect = nullptr)
			: cost(cost), name(name), preconditions(preconditions), postconditions(postconditions), effect(effect) {}

		Action(unsigned cost, std::string name, ConditionSet preconditions, ProceduralConditionList proceduralPreconditions, ConditionSet postconditions, std::function<void()> effect = nullptr)
			: cost(cost), name(name), preconditions(preconditions), proceduralPreconditions(proceduralPreconditions), postconditions(postconditions), effect(effect) {}

		Action(unsigned cost, std::string name, ConditionSet preconditions, ConditionSet postconditions, ProceduralConditionList proceduralPostconditions, std::function<void()> effect = nullptr)
			: cost(cost), name(name), preconditions(preconditions), postconditions(postconditions), proceduralPostconditions(proceduralPostconditions), effect(effect) {}

		Action(unsigned cost, std::string name, ConditionSet preconditions, ProceduralConditionList proceduralPreconditions, ConditionSet postconditions, ProceduralConditionList proceduralPostconditions, std::function<void()> effect = nullptr)
			: cost(cost), name(name), preconditions(preconditions), proceduralPreconditions(proceduralPreconditions), postconditions(postconditions), proceduralPostconditions(proceduralPostconditions), effect(effect) {}
	};

	/*! Data to be held by the ActionGraph. */
	struct ActionGraphData
	{
		std::vector<ActionList> plans; /*!< A `std::vector` holding all valid plans. */
	};

	/*! Implements goal oriented action planning. */
	class Planner
	{
	public:
		Planner() {}	/*!< Default Constructor. */
		~Planner() {}	/*!< Default Destructor. */

		/*!	Plans a series of Action%s that satisfy a given goal using goal oriented action planning.
		*
		*	@param	worldState			The starting world state.
		*	@param	goal				A desired goal represented by an Action that can only run if its preconditions have been met.
		*	@param	availableActions	A list of possible actions the planner can use to reach the goal.	
		*
		*	@return	An ActionList holding the sequence of Action%s that will reach a goal state when performed in order.
		*/
		ActionList plan(ConditionSet worldState, Action goal, ActionList availableActions);

		/*!	Saves the last generated ActionGraph as a DOT file.	
		*	Red nodes represent failed paths, green nodes represent successful, node are named after the Action they represent and edges are labeled 
		*	with the values for f(), g() and h().
		*
		*	@param	filename	The name to save the DOT file with
		*/
		void toDOT(std::string filename);

	private:
		ConditionSet _worldState;
		ActionGraph _lastPlan;

		void genTree(ActionGraph *g, int where, ActionList actions, ConditionSet currentState, ConditionSet goalState);
	};
}