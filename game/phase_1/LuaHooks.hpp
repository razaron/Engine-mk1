#pragma once
#include "Planner.hpp"

#pragma warning(push)
#pragma warning(disable: 4996)

#include <sol.hpp>
#include <glm/glm.hpp>

#pragma warning(pop)

namespace razaron::lua::planner
{
	inline void hook(sol::state_view lua)
	{
		using namespace razaron::planner;

		lua["OPERATION"] = sol::new_table();
		lua["OPERATION"]["NONE"] = Operation::NONE;
		lua["OPERATION"]["EQUAL"] = Operation::EQUAL;
		lua["OPERATION"]["LESS"] = Operation::LESS;
		lua["OPERATION"]["LESS_EQUAL"] = Operation::LESS_EQUAL;
		lua["OPERATION"]["GREATER"] = Operation::GREATER;
		lua["OPERATION"]["GREATER_EQUAL"] = Operation::GREATER_EQUAL;
		lua["OPERATION"]["ASSIGN"] = Operation::ASSIGN;
		lua["OPERATION"]["PLUS"] = Operation::PLUS;
		lua["OPERATION"]["MINUS"] = Operation::MINUS;
		lua["OPERATION"]["TIMES"] = Operation::TIMES;
		lua["OPERATION"]["DIVIDE"] = Operation::DIVIDE;

		// TODO make OPERATION immutable

		lua["ActionSet"] = sol::overload(
			[]() { return ActionSet{}; },
			[](Action a) { return ActionSet{ a }; },
			[](Action a, Action b) { return ActionSet{ a,b }; },
			[](Action a, Action b, Action c) { return ActionSet{ a,b,c }; }
		);

		lua["ConditionSet"] = sol::overload(
			[]() { return ConditionSet{}; },
			[](Condition a) { return ConditionSet{ a }; },
			[](Condition a, Condition b) { return ConditionSet{ a,b }; },
			[](Condition a, Condition b, Condition c) { return ConditionSet{ a,b,c }; }
		);

		lua.new_usertype<Condition>("Condition",
			sol::constructors<Condition(std::string, std::string, bool), Condition(std::string, std::string, bool, Operation), Condition(std::string, std::string, bool, Operation, int)>(),
			"id", &Condition::debugID,
			"type", &Condition::debugType,
			"value", &Condition::value,
			"op", &Condition::op,
			"weight", &Condition::weight
			);

		lua.new_usertype<Action>("Action",
			sol::constructors<Action(std::string, unsigned), Action(std::string, unsigned, ConditionSet, ConditionSet)>(),
			"name", &Action::name,
			"cost", &Action::cost,
			"preconditions", &Action::preconditions,
			"postconditions", &Action::postconditions
			);

		lua.new_usertype<Planner>("Planner",
			sol::constructors<Planner(), Planner(ConditionSet)>(),
			"plan", &Planner::plan,
			"savePlan", &Planner::savePlan,
			"worldState", sol::property(&Planner::getWorldState, &Planner::setWorldState)
			);
	}
}

namespace razaron::lua::maths
{
	template <typename Vector>
	inline Vector limit(const Vector &vec, float max)
	{
		if (glm::length(vec) > max)
			return glm::normalize(vec) * max;
		else
			return vec;
	}
	
	inline void hook(sol::state_view lua)
	{
		// GLM hooks
		lua["glm"] = sol::new_table{};
		sol::table table = lua["glm"];

		table.new_usertype<glm::vec2>("vec2",
			sol::constructors<glm::vec2(float, float)>(),
			sol::meta_function::addition, [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs + rhs; },
			sol::meta_function::subtraction, [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs - rhs; },
			sol::meta_function::multiplication, sol::overload(
				[](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs * rhs; },
				[](const glm::vec2 &lhs, const float rhs) { return lhs * rhs; }
			),
			sol::meta_function::division, sol::overload(
				[](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs / rhs; },
				[](const glm::vec2 &lhs, const float rhs) { return lhs / rhs; }
			),
			"x", &glm::vec2::x,
			"y", &glm::vec2::y
			);

		table["normalize"] = &glm::normalize<2, float, glm::packed_highp>;
		table["length"] = &glm::length<2, float, glm::packed_highp>;
		table["limit"] = &razaron::lua::maths::limit<glm::vec2>;
		table["dot"] = &glm::dot<2, float, glm::packed_highp>;

		table.new_usertype<glm::u8vec3>("u8vec3", sol::constructors<glm::u8vec3(unsigned char, unsigned char, unsigned char)>());
	}
}