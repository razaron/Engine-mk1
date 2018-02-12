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
	/*! The range of Operation%s available to the Planner. */
	enum class Operation
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
	using ActionSet = std::list<Action>;

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
		int weight;				/*!< A weight applied to the result of distToSatisfy. */

		std::string debugID;
		std::string debugType;

		Condition() noexcept :id{}, type{}, value{}, op{} {}
		Condition(std::string id, std::string type, ConditionValue value, Operation op = Operation::NONE, int weight = 1)
			:id{ std::hash<std::string>{}(id) }, type{ std::hash<std::string>{}(type) }, value{ value }, op{ op }, weight{ weight }, debugID{ id }, debugType{ type } {}

		bool operator==(const Condition &rhs) const noexcept
		{
			return id == rhs.id && type == rhs.type;
		}

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
			const auto index = value.index();

			switch (index)
			{
				case 0:
				{
					apply(std::get<bool>(value), modifier);
					break;
				}
				case 1:
				{
					apply(std::get<int>(value), modifier);
					break;
				}
				default:
				{
					throw std::runtime_error("This should be impossible.");
				}
			}
		}

		template <typename T>
		void apply(T &v, const Condition &modifier)
		{
			const T m = std::get<T>(modifier.value);

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
					if (v != m)
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

		void apply(bool &v, const Condition &modifier)
		{
			const bool m = std::get<bool>(modifier.value);

			switch (modifier.op)
			{
				case Operation::ASSIGN:
				{
					v = m;
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
			const auto index = value.index();

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
				default:
				{
					throw std::runtime_error("This should be impossible.");
				}
			}
		}

		template <typename T>
		bool satisfies(const Condition &condition) const
		{
			const T v = std::get<T>(value);
			const T c = std::get<T>(condition.value);

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
			const auto index = value.index();

			switch (index)
			{
				case 0:
				{
					const bool v = std::get<bool>(value);
					const bool c = std::get<bool>(condition.value);
					return (c != v)*condition.weight;
					break;
				}
				case 1:
				{
					return distToSatisfy<int>(condition);
					break;
				}
				default:
				{
					throw std::runtime_error("This should be impossible.");
				}
			}
		}

		template <typename T>
		int distToSatisfy(const Condition &condition) const
		{
			const T v = std::get<T>(value);
			const T c = std::get<T>(condition.value);

			switch (condition.op)
			{
				case Operation::EQUAL:
				{
					return (std::abs(v - c))*condition.weight;
					break;
				}
				case Operation::LESS:
				{
					if (v < c)
						return 0;
					else
						return (std::abs(v - c) + 1)*condition.weight;
					break;
				}
				case Operation::LESS_EQUAL:
				{
					if (v <= c)
						return 0;
					else
						return (std::abs(v - c))*condition.weight;
					break;
				}
				case Operation::GREATER:
				{
					if (v > c)
						return 0;
					else
						return (std::abs(v - c) + 1)*condition.weight;
					break;
				}
				case Operation::GREATER_EQUAL:
				{
					if (v >= c)
						return 0;
					else
						return (std::abs(v - c))*condition.weight;
					break;
				}
				default:
				{
					return 0;
					break;
				}
			}
		}

		void zero() noexcept
		{
			const auto index = value.index();

			switch (index)
			{
				case 0:
				{
					value = ConditionValue{ bool{} };
					break;
				}
				case 1:
				{
					value = ConditionValue{ int{} };
					break;
				}
			}
		}

	};

	/*! @cond */
	struct ConditionHash
	{
		std::size_t operator()(const Condition& p) const noexcept
		{
			return p.id ^ (p.type ^ (static_cast<std::size_t>(p.op) << 1));
		}
	};

	struct ConditionEqual
	{
		bool operator()(const Condition& lhs, const Condition& rhs) const noexcept
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

		Action(std::string name = "DEFAULT", unsigned cost = unsigned{}, ConditionSet preconditions = ConditionSet{}, ConditionSet postconditions = ConditionSet{}) noexcept
			: name{ name }, cost{ cost }, preconditions{ preconditions }, postconditions{ postconditions } {}
	};

	/*! @cond */
	struct Node
	{
		unsigned short id{};
		int f{}, g{}, h{};
		Action action{};
		ConditionSet currentState{}; // Operation::NONE for all.
		ConditionSet goalState{};
		Node *parent{};

		bool operator==(const Node &rhs) const noexcept
		{
			return id == rhs.id;
		}
	};

	using NodeList = std::list<Node>;

	using ActionGraph = razaron::graph::Graph<Node, int, int>;
	using ActionVertex = razaron::graph::Vertex<Node, int>;
	using ActionEdge = razaron::graph::Edge<int>;
	/*! @endcond */

	/*! Implements goal oriented action planning.
	*	For more information and examples, see page \ref planner
	*/
	class Planner
	{
	public:
		Planner() noexcept							/*!< Default Constructor. */
			: _worldState{}, _validNodes{}, _lastPlan{}, _nextID{ 0 } {}

		Planner(const ConditionSet &worldState)		/*!< Constructs the Planner with the passed initial world state. */
			: _worldState{ worldState }, _validNodes{}, _lastPlan{}, _nextID{ 0 } {}

		/*!	Determines the series of Action%s that will take the world state from it's current state to the state required by the goal.
		*
		*	@param	actions		The Action%s available for the Planner to use to formulate plans.
		*	@param	goal		An Action that can only run in the desired goal state.
		*
		*	@return	ActionSet	The series of actions that will take you from the current world state to the goal world state.
		*/
		ActionSet plan(ActionSet actions, Action goal);

		/*!	Saves the graph of Action%s generated during the last call to `Planner::plan`.
		*	The graph is saved in the DOT language.
		*
		*	@param	filename	The filename to save the file to.
		*/
		void savePlan(std::string filename);

		ConditionSet &getWorldState() noexcept { return _worldState; };
		ConditionSet &setWorldState(const ConditionSet &worldState) { return _worldState = worldState; };

	private:
		NodeList genAdjacent(Node * parent, ActionSet actions);
		int calculateDistanceToGoal(const ConditionSet & current, const ConditionSet & goal);

		ConditionSet _worldState;
		NodeList _validNodes;
		ActionGraph _lastPlan;
		unsigned short _nextID;
	};
}
