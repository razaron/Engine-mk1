#include "Planner.hpp"

using namespace rz::planner;
using namespace rz::graph;

ActionSet Planner::plan(ActionSet actions, Action goal)
{
    for (auto &[ws, g, p] : _oldPlans)
    {
        if (g == goal && std::equal(ws.begin(), ws.end(), _worldState.begin(), _worldState.end(), [](const Condition &a, const Condition &b) { return a.id == b.id && a.type == b.type && a.value == b.value; }))
        {
            return p;
        }
    }

    // Reset
    _validNodes.clear();
    _lastPlan = ActionGraph{};
    _nextID = 0;

    NodeList openList;
    NodeList closedList;

    Node init;
    init.id = _nextID++;
    init.action = goal;
    init.goalState = goal.preconditions;
    init.currentState = _worldState;

    _lastPlan.addEdge(0, 0);

    _lastPlan[0].data = init;
    _lastPlan[0].adjacencyList.clear();

    openList.push_back(init);

    while (openList.size())
    {
        closedList.splice(closedList.begin(), openList, openList.begin());

        Node *parent = &closedList.front();

        // If the cheapest openList node (parent) has a higher f than the cheapest valid plan
        if (_validNodes.size() && parent->f >= _validNodes.begin()->f)
            break;

        auto adjacent = genAdjacent(parent, actions);

        openList.merge(adjacent, [](const Node &a, const Node &b) {
            if (a.f <= b.f && a.h < b.h)
                return true;
            else
                return false;
        });
    }

    _validNodes.sort([](const Node &a, const Node &b) {
        if (a.f < b.f || (a.f == b.f && a.h < b.h))
            return true;
        else
            return false;
    });

    ActionSet plan;

    if (_validNodes.size())
    {
        Node cur = _validNodes.front();
        while (cur.parent)
        {
            plan.push_back(cur.action);
            cur = *cur.parent;
        }
    }

    if (plan.size())
    {
        _oldPlans.push_back(std::make_tuple(_worldState, goal, plan));

        if (_oldPlans.size() > 10)
            _oldPlans.erase(_oldPlans.begin());
    }

    return plan;
}

void Planner::savePlan(std::string filename)
{
    _lastPlan.vertexFuncs[State::WHITE] = [&](ActionVertex &v, ActionGraph &) {
        if (!v.adjacencyList.size())
        {
            if (std::count(_validNodes.begin(), _validNodes.end(), v.data))
                v.state = State::GREEN;
            else
                v.state = State::RED;
        }
    };

    _lastPlan.breadthFirstTraversal(0);

    std::map<Operation, std::string> opMap{
        { Operation::ASSIGN, "=" },
        { Operation::PLUS, "+" },
        { Operation::MINUS, "-" },
        { Operation::TIMES, "*" },
        { Operation::DIVIDE, "/" },
        { Operation::EQUAL, "==" },
        { Operation::GREATER, ">" },
        { Operation::GREATER_EQUAL, ">=" },
        { Operation::LESS, "<" },
        { Operation::LESS_EQUAL, "<=" },
        { Operation::NONE, ":" }
    };

    auto getValue = [](Condition &cond) {
        switch (cond.value.index())
        {
            case 0:
            {
                return std::string{ (std::get<bool>(cond.value)) ? "true" : "false" };
            }
            case 1:
            {
                return std::to_string(std::get<int>(cond.value));
            }
            default:
            {
                throw std::runtime_error("This should be impossible.");
            }
        }
    };

    const auto vAttr = [&opMap, &getValue](const ActionVertex &v) {
        const auto id = v.id;
        const auto name = v.data.action.name;

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
            case State::WHITE:
            {
                colour = "white";
                break;
            }

            case State::BLACK:
            {
                colour = "black";
                break;
            }
        }

        std::stringstream attributes;
        attributes << std::to_string(id) << "[label=\"" << name << "\" tooltip=\"";

        attributes << "Current State:\\n";
        for (auto cond : v.data.currentState)
        {
            attributes << "&#9;&#9;&#9;&#9;" << cond.debugID << "(" << cond.debugType << ") " << opMap[cond.op] << " " << getValue(cond) << "\\n";
        }

        attributes << "Goal State:\\n";
        for (auto cond : v.data.goalState)
        {
            attributes << "&#9;&#9;&#9;&#9;" << cond.debugID << "(" << cond.debugType << ") " << opMap[cond.op] << " " << getValue(cond) << "\\n";
        }

        attributes << "\\nPre-Conditions:\\n";
        for (auto cond : v.data.action.preconditions)
        {
            attributes << "&#9;&#9;&#9;&#9;" << cond.debugID << "(" << cond.debugType << ") " << opMap[cond.op] << " " << getValue(cond) << "\\n";
        }

        attributes << "Post-Conditions:\\n";
        for (auto cond : v.data.action.postconditions)
        {
            attributes << "&#9;&#9;&#9;&#9;" << cond.debugID << "(" << cond.debugType << ") " << opMap[cond.op] << " " << getValue(cond) << "\\n";
        }

        attributes << "\" style=filled fillcolor=" << colour << "]";

        return attributes.str();
    };

    const auto eAttr = [&](const ActionEdge &e) {
        std::stringstream attr;

        auto node = _lastPlan[e.target].data;

        attr << "[dir=back label=\"F(" << node.f << ")\" labeltooltip=\"G(" << node.g << ")\\nH(" << node.h << ")\"]";

        return attr.str();
    };

    std::ofstream dotFile(filename);
    auto dotStr = _lastPlan.getDOT(vAttr, eAttr);

    dotFile << dotStr;
}

NodeList Planner::genAdjacent(Node *parent, ActionSet actions)
{
    NodeList list;

    for (auto &a : actions)
    {
        // Add postconditions to current state
        ConditionSet current{ parent->currentState };
        for (auto &cond : a.postconditions)
        {
            auto it = std::find(current.begin(), current.end(), cond);

            if (it != current.end())
                it->apply(cond);
            else
            {
                // Create a new zeroed Condition with the id and type of cond
                Condition prop{ cond };
                prop.zero();
                prop.op = Operation::NONE;

                prop.apply(cond);

                current.push_back(prop);
            }
        }

        // Add procedural postconditions to current state
        for (auto &func : a.procPostconditions)
        {
            auto cond = func();
            auto it = std::find(current.begin(), current.end(), cond);

            if (it != current.end())
                it->apply(cond);
            else
            {
                // Create a new zeroed Condition with the id and type of cond
                Condition prop{ cond };
                prop.zero();
                prop.op = Operation::NONE;

                prop.apply(cond);

                current.push_back(prop);
            }
        }

        // Calculate distance to goals before and after
        int before = calculateDistanceToGoal(parent->goalState, parent->currentState);
        int after = calculateDistanceToGoal(parent->goalState, current);

        if (after < before)
        {
            // Add preconditions to the goal state
            ConditionSet goal{ parent->goalState };
            for (auto &cond : a.preconditions)
            {
                auto it = std::find(goal.begin(), goal.end(), cond);

                if (it != goal.end())
                    it->apply(cond);
                else
                    goal.push_back(cond);
            }

            // Add procedural preconditions to the goal state
            for (auto &func : a.procPreconditions)
            {
                auto cond = func();
                auto it = std::find(goal.begin(), goal.end(), cond);

                if (it != goal.end())
                    it->apply(cond);
                else
                    goal.push_back(cond);
            }

            // Create Node with action
            Node n;
            n.id = _nextID++;
            n.action = a;
            n.currentState = current;
            n.goalState = goal;
            n.parent = parent;

            // Check world state satisfies preconditions
            ConditionSet temp{ a.preconditions };
            for (auto &cond : a.preconditions)
            {
                auto it = std::find(_worldState.begin(), _worldState.end(), cond);
                if (it != _worldState.end() && it->satisfies(cond))
                {
                    temp.remove_if([cond](const Condition &c) {
                        if (c == cond && c.op == cond.op && c.value == c.value)
                            return true;
                        else
                            return false;
                    });
                }
            }

            // Recalculate distance to goal
            before = after;
            after = calculateDistanceToGoal(goal, current);

            if (temp.size() == 0 && after == 0)
            {
                n.g = parent->g + a.cost;
                n.g *= PLANNER_G_MOD;
                n.h = 0;
                n.h *= PLANNER_H_MOD;
                n.f = n.g + n.h;

                _validNodes.push_back(n);

                _lastPlan.addEdge(parent->id, n.id, n.f);
                _lastPlan[n.id].data = n;
            }
            else
            {
                n.g = parent->g + a.cost;
                n.g *= PLANNER_G_MOD;
                n.h = after;
                n.h *= PLANNER_H_MOD;
                n.f = n.g + n.h;

                list.push_back(n);

                _lastPlan.addEdge(parent->id, n.id, n.f);
                _lastPlan[n.id].data = n;
            }
        }
    }

    // sort the list
    if (list.size() > 1)
    {
        list.sort([](const Node &a, const Node &b) {
            if (a.f < b.f || (a.f == b.f && a.h < b.h))
                return true;
            else
                return false;
        });
    }

    return list;
}

int Planner::calculateDistanceToGoal(const ConditionSet &goal, const ConditionSet &current)
{
    int dist = 0;
    for (auto &cond : goal)
    {
        auto it = std::find(current.begin(), current.end(), cond);
        if (it != current.end())
        {
            dist += it->distToSatisfy(cond);
        }
        else
        {
            Condition prop{ cond };
            prop.zero();
            prop.op = Operation::NONE;

            dist += prop.distToSatisfy(cond);
        }
    }

    return dist;
}
