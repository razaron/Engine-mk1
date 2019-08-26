#ifndef RZ_FRAMEWORK_LUA_API_HPP
#define RZ_FRAMEWORK_LUA_API_HPP

#include "EventStream.hpp"
#include "Planner.hpp"

#include "sol.hpp"

namespace rz::lua
{
    namespace planner
    {
        void bind(sol::state_view lua)
        {
            using namespace rz::planner;

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
                [](Action a, Action b) { return ActionSet{ a, b }; },
                [](Action a, Action b, Action c) { return ActionSet{ a, b, c }; });

            lua["ConditionSet"] = sol::overload(
                []() { return ConditionSet{}; },
                [](Condition a) { return ConditionSet{ a }; },
                [](Condition a, Condition b) { return ConditionSet{ a, b }; },
                [](Condition a, Condition b, Condition c) { return ConditionSet{ a, b, c }; });

            lua["ProceduralConditionSet"] = sol::overload(
                []() { return ProceduralConditionSet{}; },
                [](ProceduralCondition a) { return ProceduralConditionSet{ a }; },
                [](ProceduralCondition a, ProceduralCondition b) { return ProceduralConditionSet{ a, b }; },
                [](ProceduralCondition a, ProceduralCondition b, ProceduralCondition c) { return ProceduralConditionSet{ a, b, c }; });

            lua.new_usertype<Condition>("Condition",
                                        sol::constructors<
                                            Condition(std::string, std::string, bool),
                                            Condition(std::string, std::string, bool, Operation),
                                            Condition(std::string, std::string, bool, Operation, int),
                                            Condition(std::string, std::string, int),
                                            Condition(std::string, std::string, int, Operation),
                                            Condition(std::string, std::string, int, Operation, int)>(),
                                        "id", &Condition::debugID,
                                        "type", &Condition::debugType,
                                        "value", &Condition::value,
                                        "op", &Condition::op,
                                        "weight", &Condition::weight);

            lua.new_usertype<Action>("Action",
                                     sol::constructors<Action(std::string, unsigned), Action(std::string, unsigned, ConditionSet, ConditionSet), Action(std::string, unsigned, ConditionSet, ConditionSet, ProceduralConditionSet, ProceduralConditionSet)>(),
                                     "name", &Action::name,
                                     "cost", &Action::cost,
                                     "preconditions", &Action::preconditions,
                                     "postconditions", &Action::postconditions);

            lua.new_usertype<Planner>("Planner",
                                      sol::constructors<Planner(), Planner(ConditionSet)>(),
                                      "plan", &Planner::plan,
                                      "savePlan", &Planner::savePlan,
                                      "worldState", sol::property(&Planner::getWorldState, &Planner::setWorldState));
        }
    } // namespace planner

    namespace eventstream
    {
        void bind(sol::state_view lua)
        {
            using namespace rz::eventstream;

            // NOTE: sol3 apparently binds certain operators manually
            lua.new_usertype<Event>("Event",
                                    sol::constructors<Event(), Event(UUID64, EventType), Event(UUID64, EventType, std::shared_ptr<void>), Event(UUID64, EventType, std::shared_ptr<void>, unsigned)>(),
                                    "recipient", &Event::recipient,
                                    "type", &Event::type,
                                    "data", &Event::data,
                                    "lifetime", &Event::lifetime,
                                    "id", &Event::id);

            lua.new_usertype<EventStream>("EventStream",
                                          sol::constructors<EventStream()>(),
                                          "pushEvent", &EventStream::pushEvent,
                                          "popEvent", &EventStream::popEvent,
                                          "registerHandler", &EventStream::registerHandler,
                                          "extendHandler", &EventStream::extendHandler);
        }
    } // namespace eventstream

    namespace misc
    {
        void bind(sol::state_view lua)
        {
            lua.new_usertype<Handle>("Handle",
                                     sol::constructors<Handle(), Handle(HandleType)>());

            lua.new_usertype<UUID64>("UUID64",
                                     sol::constructors<UUID64(), UUID64(std::uint64_t)>());
        }
    } // namespace Misc
} // namespace rz::lua

#endif //RZ_FRAMEWORK_LUA_API_HPP
