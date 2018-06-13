#include "Planner.hpp"

#include <catch.hpp>

using namespace razaron::planner;

SCENARIO("A Condition object can behave as a property, condition or modifier.", "[planner]")
{
	GIVEN("A Condition with value of type \"int\".")
	{
		Condition foo{ "foo", "int", 5 };

		WHEN("The Condition object is a property")
		{
			Condition bar{ "bar", "int", 2 };

			bool result = false;

			try
			{
				foo.apply(bar);
			}
			catch (const std::invalid_argument&)
			{
				result = true;
			}

			REQUIRE(result == true);
		}

		WHEN("The Condition object is a condition")
		{
			Condition e{ "bar", "int", 2, Operation::EQUAL };
			Condition l{ "bar", "int", 2, Operation::LESS };
			Condition le{ "bar", "int", 2, Operation::LESS_EQUAL };
			Condition g{ "bar", "int", 2, Operation::GREATER };
			Condition ge{ "bar", "int", 2, Operation::GREATER_EQUAL };

			REQUIRE(foo.satisfies(e) == false);
			REQUIRE(foo.satisfies(l) == false);
			REQUIRE(foo.satisfies(le) == false);
			REQUIRE(foo.satisfies(g) == true);
			REQUIRE(foo.satisfies(ge) == true);
		}

		WHEN("The Condition object is a modifier")
		{
			Condition a{ "bar", "int", 2, Operation::ASSIGN };
			Condition p{ "bar", "int", 2, Operation::PLUS };
			Condition m{ "bar", "int", 2, Operation::MINUS };
			Condition t{ "bar", "int", 2, Operation::TIMES };
			Condition d{ "bar", "int", 2, Operation::DIVIDE };

			REQUIRE(std::get<int>(foo.value) == 5);
			foo.apply(a);
			REQUIRE(std::get<int>(foo.value) == 2);
			foo.apply(p);
			REQUIRE(std::get<int>(foo.value) == 2 + 2);
			foo.apply(m);
			REQUIRE(std::get<int>(foo.value) == (2 + 2) - 2);
			foo.apply(t);
			REQUIRE(std::get<int>(foo.value) == ((2 + 2) - 2) * 2);
			foo.apply(d);
			REQUIRE(std::get<int>(foo.value) == (((2 + 2) - 2) * 2) / 2);
		}
	}

	GIVEN("A Condition with value of type \"bool\".")
	{
		Condition foo{ "foo", "bool", true };

		WHEN("The Condition object is a property")
		{
			Condition bar{ "bar", "bool", false };

			bool result{ false };

			try
			{
				foo.apply(bar);
			}
			catch (const std::invalid_argument&)
			{
				result = true;
			}

			REQUIRE(result == true);
		}

		WHEN("The Condition object is a condition")
		{
			Condition e{ "bar", "bool", false, Operation::EQUAL };
			Condition l{ "bar", "bool", false, Operation::LESS };
			Condition le{ "bar", "bool", false, Operation::LESS_EQUAL };
			Condition g{ "bar", "bool", false, Operation::GREATER };
			Condition ge{ "bar", "bool", false, Operation::GREATER_EQUAL };

			REQUIRE(foo.satisfies(e) == (true == false));
			REQUIRE(foo.satisfies(l) == (true < false));
			REQUIRE(foo.satisfies(le) == (true <= false));
			REQUIRE(foo.satisfies(g) == (true > false));
			REQUIRE(foo.satisfies(ge) == (true >= false));
		}

		WHEN("The Condition object is a modifier")
		{
			Condition a{ "bar", "bool", false, Operation::ASSIGN };
			Condition p{ "bar", "bool", false, Operation::PLUS };
			Condition m{ "bar", "bool", false, Operation::MINUS };
			Condition t{ "bar", "bool", false, Operation::TIMES };
			Condition d{ "bar", "bool", false, Operation::DIVIDE };

			REQUIRE(std::get<bool>(foo.value) == true);
			foo.apply(a);
			REQUIRE(std::get<bool>(foo.value) == false);

			bool result{ true };

			try
			{
				foo.apply(p);
				result = false;
			}
			catch (const std::invalid_argument&)
			{
			}

			try
			{
				foo.apply(m);
				result = false;
			}
			catch (const std::invalid_argument&)
			{
			}

			try
			{
				foo.apply(t);
				result = false;
			}
			catch (const std::invalid_argument&)
			{
			}

			try
			{
				foo.apply(d);
				result = false;
			}
			catch (const std::invalid_argument&)
			{
			}

			REQUIRE(result == true);
		}
	}
}

SCENARIO("Planners can determine the optimal sequence of Actions to reach a goal state from a given worldstate.", "[planner]")
{
	ActionSet actions;
	actions.push_back(Action{ "MINE_METAL",2,
		ConditionSet{
			Condition{ "PLAYER", "HAS_PICK", 0, Operation::GREATER_EQUAL }
		},
		ConditionSet{
			Condition{ "INVENTORY", "METAL", 1, Operation::PLUS },
			Condition{ "PLAYER", "HAS_PICK", 1, Operation::MINUS }
		}
		});

	actions.push_back(Action{ "GET_PICK",1,
		ConditionSet{
			Condition{ "INVENTORY", "TOOL", 0, Operation::GREATER_EQUAL }
		},
		ConditionSet{
			Condition{ "PLAYER", "HAS_PICK", 1, Operation::PLUS },
			Condition{ "INVENTORY", "TOOL", 1, Operation::MINUS }
		}
		});

	actions.push_back(Action{ "MINE_WOOD",2,
		ConditionSet{
			Condition{ "PLAYER", "HAS_AXE", 0, Operation::GREATER_EQUAL }
		},
		ConditionSet{
			Condition{ "INVENTORY", "WOOD", 1, Operation::PLUS },
			Condition{ "PLAYER", "HAS_AXE", 1, Operation::MINUS }
		}
		});

	actions.push_back(Action{ "GET_AXE",1,
		ConditionSet{
			Condition{ "INVENTORY", "TOOL", 0, Operation::GREATER_EQUAL }
		},
		ConditionSet{
			Condition{ "PLAYER", "HAS_AXE", 1, Operation::PLUS },
			Condition{ "INVENTORY", "TOOL", 1, Operation::MINUS }
		}
		});

	actions.push_back(Action{ "PURCHASE_METAL",4,
		ConditionSet{},
		ConditionSet{
			Condition{ "INVENTORY", "METAL", 1, Operation::PLUS }
		}
		});

	actions.push_back(Action{ "PURCHASE_WOOD",4,
		ConditionSet{},
		ConditionSet{
			Condition{ "INVENTORY", "WOOD", 1, Operation::PLUS }
		}
		});

	actions.push_back(Action{ "CRAFT_TOOL", 2,
		ConditionSet{
			Condition{ "INVENTORY", "WOOD", 0, Operation::GREATER_EQUAL },
			Condition{ "INVENTORY", "METAL", 0, Operation::GREATER_EQUAL }
		},
		ConditionSet{
			Condition{ "INVENTORY", "TOOL", 1, Operation::PLUS },
			Condition{ "INVENTORY", "WOOD", 1, Operation::MINUS },
			Condition{ "INVENTORY", "METAL", 1, Operation::MINUS }
		}
		});

	actions.push_back(Action{ "DRINK_TEA",1,
		ConditionSet{},
		ConditionSet{}
		});

	Planner planner;

	WHEN("Attempting to acquire 2 tools.")
	{
		Action goal{ "GOAL", 0,
			ConditionSet{
				Condition{ "INVENTORY", "TOOL", 2, Operation::GREATER_EQUAL, 5 }
			}
		};

		GIVEN("Worldstate: Default.")
		{
			auto plan = planner.plan(actions, goal);

			REQUIRE(plan.size() == 6);

			auto result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_METAL" }; });
			REQUIRE(result == 2);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_WOOD" }; });
			REQUIRE(result == 2);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "CRAFT_TOOL" }; });
			REQUIRE(result == 2);
		}

		GIVEN("Worldstate: One tool available.")
		{
			ConditionSet ws;
			ws.push_back(Condition{ "INVENTORY", "TOOL", 1 });

			planner.setWorldState(ws);

			auto plan = planner.plan(actions, goal);

			REQUIRE(plan.size() == 3);

			auto result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_METAL" }; });
			REQUIRE(result == 1);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_WOOD" }; });
			REQUIRE(result == 1);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "CRAFT_TOOL" }; });
			REQUIRE(result == 1);
		}

		GIVEN("Worldstate: Two axes equipped.")
		{
			ConditionSet ws;
			ws.push_back(Condition{ "PLAYER", "HAS_AXE", 2 });

			planner.setWorldState(ws);

			auto plan = planner.plan(actions, goal);

			REQUIRE(plan.size() == 6);

			auto result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_METAL" }; });
			REQUIRE(result == 2);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "MINE_WOOD" }; });
			REQUIRE(result == 2);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "CRAFT_TOOL" }; });
			REQUIRE(result == 2);
		}
	}

	WHEN("Attempting to have exactly 1 axe.")
	{
		Action goal{ "GOAL", 0,
			ConditionSet{
			Condition{ "PLAYER", "HAS_AXE", 1, Operation::EQUAL, 5 }
		}
		};

		GIVEN("Worldstate: Default.")
		{
			auto plan = planner.plan(actions, goal);

			REQUIRE(plan.size() == 4);

			auto result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_METAL" }; });
			REQUIRE(result == 1);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "PURCHASE_WOOD" }; });
			REQUIRE(result == 1);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "CRAFT_TOOL" }; });
			REQUIRE(result == 1);

			result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "GET_AXE" }; });
			REQUIRE(result == 1);
		}

		GIVEN("Worldstate: One tool available.")
		{
			ConditionSet ws;
			ws.push_back(Condition{ "INVENTORY", "TOOL", 1 });

			planner.setWorldState(ws);

			auto plan = planner.plan(actions, goal);

			REQUIRE(plan.size() == 1);

			auto result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "GET_AXE" }; });
			REQUIRE(result == 1);
		}

		GIVEN("Worldstate: Two axes equipped.")
		{
			ConditionSet ws;
			ws.push_back(Condition{ "PLAYER", "HAS_AXE", 2 });

			planner.setWorldState(ws);

			auto plan = planner.plan(actions, goal);

			REQUIRE(plan.size() == 1);

			auto result = std::count_if(plan.begin(), plan.end(), [](const Action &a) {return a.name == std::string{ "MINE_WOOD" }; });
			REQUIRE(result == 1);
		}
	}
}