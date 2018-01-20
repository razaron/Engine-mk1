#pragma once
#include "Planner.hpp"

#include <sol.hpp>

namespace razaron::lua::planner
{
	using namespace razaron::planner;

	inline void hook(sol::state_view lua)
	{
		lua["ConditionSet"] = sol::overload(
			[]() { return ConditionSet{}; },
			[](const char* a) { return ConditionSet{ a }; },
			[](const char* a, const char* b) { return ConditionSet{ a,b }; },
			[](const char* a, const char* b, const char* c) { return ConditionSet{ a,b,c }; }
		);

		lua["ProceduralConditionList"] = sol::overload(
			[]() { return ProceduralConditionList{}; },
			[](ProceduralCondition a) { return ProceduralConditionList{ a }; },
			[](ProceduralCondition a, ProceduralCondition b) { return ProceduralConditionList{ a,b }; },
			[](ProceduralCondition a, ProceduralCondition b, ProceduralCondition c) { return ProceduralConditionList{ a,b,c }; }
		);

		lua["ActionList"] = sol::overload(
			[]() { return ActionList{}; },
			[](Action a) { return ActionList{ a }; },
			[](Action a, Action b) { return ActionList{ a,b }; },
			[](Action a, Action b, Action c) { return ActionList{ a,b,c }; }
		);

		lua.new_usertype<Action>("Action",
			sol::constructors<Action(), Action(unsigned, std::string, ConditionSet, ConditionSet, std::function<void()>), Action(unsigned, std::string, ConditionSet, ProceduralConditionList, ConditionSet, std::function<void()>), Action(unsigned, std::string, ConditionSet, ConditionSet, ProceduralConditionList, std::function<void()>), Action(unsigned, std::string, ConditionSet, ProceduralConditionList, ConditionSet, ProceduralConditionList, std::function<void()>)>(),
			"cost", sol::readonly(&Action::cost),
			"name", sol::readonly(&Action::name),
			"preconditions", &Action::preconditions,
			"proceduralPreconditions", &Action::proceduralPreconditions,
			"postconditions", &Action::postconditions,
			"proceduralPostconditions", &Action::proceduralPostconditions
			);

		lua.new_usertype<Planner>("Planner",
			sol::constructors<Planner(), Planner(ConditionSet, std::function<void()>)>(),
			"plan", &Planner::plan
			);
	}
}