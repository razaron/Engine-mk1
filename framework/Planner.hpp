#pragma once

#include "Graph.hpp"
#include "Misc.hpp"

#include <unordered_set>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <variant>

#ifndef PLANNER_G_MOD
	#define PLANNER_G_MOD 1
#endif

#ifndef PLANNER_H_MOD
	#define PLANNER_H_MOD 1
#endif

/*! Planners are used to generate a sequence of Action%s that satisfy a goal */
namespace razaron::planner
{
	using namespace razaron::graph;

	/*! The range of Operation%s available to the Planner. */
	enum class Operation : std::size_t
	{
		NONE,
		EQUAL,
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL,
		ASSIGN,
		PLUS,
		MINUS,
		TIMES,
		DIVIDE
	};

	struct Action;
	struct Condition;

	using ConditionSet = std::list<Condition>;
	using ActionList = std::list<Action>;

	/*! Represents a world state property, condition or modifier in a format interpretable by the planner. 
	*	Properties have an `op` value of `Operation::NONE` while all other `op` values denote a condition or modifier.
	*/
	struct Condition
	{
		using ConditionValue = std::variant<bool, int>;

		std::size_t id;			/*!< A hashed `std::string` denoting the property owner. */
		std::size_t type;		/*!< A hashed `std::string` denoting the property type. */
		ConditionValue value;	/*!< A `std::variant` denoting the value off the property. */
		Operation op;			/*!< The Operation used to evaluate or apply a condition. */

		std::string debugID;
		std::string debugType;

		Condition() :id(0), type(0), value(false) {}
		Condition(std::string id, std::string type, ConditionValue value, Operation op = Operation::NONE)
			:id(std::hash<std::string>{}(id)), type(std::hash<std::string>{}(type)), value(value), op(op), debugID(id), debugType(type) {}

		/*!	Applies a modifier to this property. Can also be used to add conditions (e.g. `x>2` + `x>4` = `x>6`).
		*	Follows the format: `this.value modifier.op modifier.value`. 
		*	E.g. If `this.value = 5`, `modifier.op = Operation::PLUS` and `modifier.value = 2` the function would perform `this.value = 5 + 2`.
		*
		*	@param		modifier				The modifier to apply to this condition or property.
		*
		*	@exception	std::logic_error		Passed a Condition with `op == Operation::EQUAL` and `value != this.value`
		*	@exception	std::invalid_argument	Passed a Condition with `op == Operation::NONE`.
		*
		*/
		void apply(const Condition &modifier)
		{
			auto index = value.index();

			switch (index)
			{
			case 0:
			{
				apply<bool>(modifier);
				break;
			}
			case 1:
			{
				apply<int>(modifier);
				break;
			}
			}
		}

		template <typename T>
		void apply(const Condition &modifier)
		{
			T v = std::get<T>(value);
			T m = std::get<T>(modifier.value);

			switch (modifier.op)
			{
			case Operation::ASSIGN:
			{
				v = m;
				break;
			}
			case Operation::PLUS:
			{
				v = v + m;
				break;
			}
			case Operation::MINUS:
			{
				v = v - m;
				break;
			}
			case Operation::TIMES:
			{
				v = v * m;
				break;
			}
			case Operation::DIVIDE:
			{
				v = v / m;
				break;
			}
			case Operation::LESS:
			{
				v = v - m;
				break;
			}
			case Operation::LESS_EQUAL:
			{
				v = v - m;
				break;
			}
			case Operation::GREATER:
			{
				v = v + m;
				break;
			}
			case Operation::GREATER_EQUAL:
			{
				v = v + m;
				break;
			}
			case Operation::EQUAL:
			{
				if(v != m)
					throw std::logic_error("Invalid operation in Condition::apply. Logical falicy.");

				v = v;
				break;
			}
			default:
			{
				throw std::invalid_argument("Invalid operation in Condition::apply. Operation not handled.");
				break;
			}
			}

			value = ConditionValue{ v };
		}

		/*	Evaluates a condition using this against this property.
		*	Follows the format: `this.value modifier.op modifier.value`. 
		*	E.g. If `this.value = 5`, `modifier.op = Operation::PLUS` and `modifier.value = 2` the function would perform `this.value = 5 + 2`.
		*
		*	@param		condition				The condition to evaluate against this property.
		*
		*	@exception	std::invalid_argument	Passed condition uses a non-comparison Operator.
		*
		*	@return		bool					True if this property satisfies the condition. Else false.
		*/
		bool satisfies(const Condition &condition) const
		{
			auto index = value.index();

			switch (index)
			{
			case 0:
			{
				return satisfies<bool>(condition);
				break;
			}
			case 1:
			{
				return satisfies<int>(condition);
				break;
			}
			}
		}

		template <typename T>
		bool satisfies(const Condition &condition) const
		{
			T v = std::get<T>(value);
			T c = std::get<T>(condition.value);

			switch (condition.op)
			{
			case Operation::EQUAL:
			{
				return v == c;
				break;
			}
			case Operation::LESS:
			{
				return v < c;
				break;
			}
			case Operation::LESS_EQUAL:
			{
				return v <= c;
				break;
			}
			case Operation::GREATER:
			{
				return v > c;
				break;
			}
			case Operation::GREATER_EQUAL:
			{
				return v >= c;
				break;
			}
			default:
			{
				throw std::invalid_argument("Invalid operation in Condition::satisfies.");
				break;
			}
			}
		}


		/*	Evaluates how close a condition is to being satisfied by this property.
		*	The distance represents the minimum value this property needs to be modified by in order to satisfy the condition.
		*
		*	@param		condition				The condition to calculate the distance to.
		*
		*	@return		int						The minimum distance to the condition.
		*/
		int distToSatisfy(const Condition &condition) const
		{
			auto index = value.index();

			switch (index)
			{
			case 0:
			{
				bool c = std::get<bool>(value);
				bool v = std::get<bool>(condition.value);
				return c != v;
				break;
			}
			case 1:
			{
				return distToSatisfy<int>(condition);
				break;
			}
			}
		}

		template <typename T>
		int distToSatisfy(const Condition &condition) const
		{
			T v = std::get<T>(value);
			T c = std::get<T>(condition.value);

			switch (condition.op)
			{
			case Operation::EQUAL:
			{
				return std::abs(v - c);
				break;
			}
			case Operation::LESS:
			{
				if (v < c)
					return 0;
				else
					return std::abs(v - c) + 1;
				break;
			}
			case Operation::LESS_EQUAL:
			{
				if (v <= c)
					return 0;
				else
					return std::abs(v - c);
				break;
			}
			case Operation::GREATER:
			{
				if (v > c)
					return 0;
				else
					return std::abs(v - c) + 1;
				break;
			}
			case Operation::GREATER_EQUAL:
			{
				if (v >= c)
					return 0;
				else
					return std::abs(v - c);
				break;
			}
			default:
			{
				return 0;
				break;
			}
			}
		}

		void zero()
		{
			auto index = value.index();

			switch (index)
			{
			case 0:
			{
				value = ConditionValue{ false };
				break;
			}
			case 1:
			{
				value = ConditionValue{ 0 };
				break;
			}
			}
		}

		bool operator==(const Condition &rhs) const
		{
			return id == rhs.id && type == rhs.type;
		}
	};

	/*! @cond */
	struct ConditionHash
	{
		std::size_t operator()(const Condition& p) const
		{
			return p.id ^ (p.type ^ (static_cast<std::size_t>(p.op) << 1));
		}
	};

	struct ConditionEqual
	{
		bool operator()(const Condition& lhs, const Condition& rhs) const
		{
			return lhs.id == rhs.id && lhs.type == rhs.type;
		}
	};
	/*! @endcond */

	/*! A single atomic Action used to modify the world state in some manner. */
	struct Action
	{
		std::string name;				/*!< The Action%s name. */
		unsigned cost;					/*!< The cost to perform this Action. */

		ConditionSet preconditions;		/*!< Condition%s that must be satisfied for this Action to be valid. */
		ConditionSet postconditions;	/*!< Modifiers this Action applies to the world state during planning. */

		std::function<void()> effect;	/*!< A function the runs the logic for this Action. Not used during planning. */

		Action(std::string name = "DEFAULT", unsigned cost = 0, ConditionSet preconditions = ConditionSet{}, ConditionSet postconditions = ConditionSet{}, std::function<void()> effect = nullptr)
			: name(name), cost(cost), preconditions(preconditions), postconditions(postconditions), effect(effect) {}
	};

	/*! @cond */
	struct Node
	{
		unsigned id{ 0 };
		int f{ 0 }, g{ 0 }, h{ 0 };
		Action action{};
		ConditionSet currentState{}; // Operation::NONE for all.
		ConditionSet goalState{};
		Node *parent{ nullptr };

		bool operator==(const Node &rhs) const
		{
			return id == rhs.id;
		}
	};

	using NodeList = std::list<Node>;

	using ActionGraph = Graph<Node, int, int>;
	using ActionVertex = Vertex<Node, int>;
	using ActionEdge = Edge<int>;
	/*! @endcond */

	/*! Implements goal oriented action planning.
	*	For more information and examples, see page \ref planner
	*/
	class Planner
	{
	public:
		Planner() {}															/*!< Default Constructor. */
		Planner(const ConditionSet &worldState) : _worldState(worldState) {}	/*!< Constructs the Planner with the passed initial world state. */
		~Planner() {}															/*!< Default Destructor. */

		/*!	Determines the series of Action%s that will take the world state from it's current state to the state required by the goal.
		*
		*	@param	actions		The Action%s available for the Planner to use to formulate plans.
		*	@param	goal		An Action that can only run in the desired goal state.
		*
		*	@return	ActionList	The series of actions that will take you from the current world state to the goal world state.
		*/
		ActionList plan(ActionList actions, Action goal);

		/*!	Saves the graph of Action%s generated during the last call to `Planner::plan`.
		*	The graph is saved in the DOT language.
		*
		*	@param	filename	The filename to save the file to.
		*/
		void toDOT(std::string filename);
		
		ConditionSet &getWorldState() { return _worldState; };
		ConditionSet &setWorldState(const ConditionSet &worldState) { return _worldState = worldState; };

	private:
		ConditionSet _worldState;
		NodeList _validNodes;
		ActionGraph _lastPlan;
		unsigned _nextID{ 0 };

		NodeList genAdjacent(Node * parent, ActionList actions);
		int calculateDistanceToGoal(const ConditionSet & current, const ConditionSet & goal);
	};
}