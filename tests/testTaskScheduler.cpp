#include <catch.hpp>

#include "TaskScheduler.hpp"

using namespace razaron::taskscheduler;

SCENARIO("TaskSchedulers can queue work in several ways", "[taskscheduler]")
{
    GIVEN("A TaskScheduler")
    {
        TaskScheduler ts;

        THEN("You can push individual work items")
        {
            std::vector<int> output;
            std::mutex outputMutex;

            WHEN("The work has no dependencies")
            {
                for (auto i = 0; i < 1000; i++)
                {
                    WorkFunc work = [&output, &outputMutex]() {
                        std::lock_guard<std::mutex> lk(outputMutex);
                        output.push_back(1);
                    };
                    ts.push(work);
                }

                ts.helpWorkers();

                REQUIRE(output.size() == 1000);
            }

            WHEN("The work does have dependencies")
            {
                Task dep{};
                for (auto i = 0; i < 1000; i++)
                {
                    WorkFunc work = [&output, &outputMutex, i]() {
                        std::lock_guard<std::mutex> lk(outputMutex);
                        output.push_back(i);
                    };
                    dep = ts.push(work, dep);
                }

                ts.helpWorkers();

                REQUIRE(output.size() == 1000);

                bool isSuccess = true;
                int prev = -1;
                for (auto &i : output)
                {
                    if (i != prev + 1)
                        isSuccess = false;

                    prev = i;
                }

                REQUIRE(isSuccess == true);
            }
        }

        THEN("You can push groups of work")
        {
            std::vector<int> output;
            std::mutex outputMutex;

            WHEN("The WorkGroup has no dependencies")
            {
                for (auto i = 0; i < 1000; i += 10)
                {
                    WorkGroup group;
                    group.second.push_back([&output, &outputMutex, i]() {
                        std::lock_guard<std::mutex> lk(outputMutex);
                        output.push_back(i);
                    });

                    // Create 9 child tasks for `current`
                    for (auto j = 0; j < 9; j++)
                    {
                        group.second.push_back([&output, &outputMutex, i]() {
                            std::lock_guard<std::mutex> lk(outputMutex);
                            output.push_back(i);
                        });
                    }

                    ts.push(group);
                }

                ts.helpWorkers();

                REQUIRE(output.size() == 1000);
            }

            WHEN("The WorkGroup does have dependencies")
            {
                Task dep{};
                for (auto i = 0; i < 1000; i += 10)
                {
                    WorkGroup group;
                    group.second.push_back([&output, &outputMutex, i]() {
                        std::lock_guard<std::mutex> lk(outputMutex);
                        output.push_back(i);
                    });

                    // Create 9 child tasks for `current`
                    for (auto j = 0; j < 9; j++)
                    {
                        group.second.push_back([&output, &outputMutex, i]() {
                            std::lock_guard<std::mutex> lk(outputMutex);
                            output.push_back(i);
                        });
                    }

                    dep = ts.push(group, dep);
                }

                ts.helpWorkers();

                REQUIRE(output.size() == 1000);

                bool isSuccess = true;
                int prev = 0;
                for (auto &i : output)
                {
                    if (i != prev + 10 && i != prev)
                        isSuccess = false;

                    prev = i;
                }

                REQUIRE(isSuccess == true);
            }
        }

        THEN("You can push dependency graphs of work")
        {
            std::vector<int> output;
            std::mutex outputMutex;

            WHEN("The WorkGraph has no dependencies")
            {
                WorkGraph g;
                for (auto i = 0u; i < 1000; i += 3)
                {
                    g.addEdge(i, i + 1);
                    g.addEdge(i + 1, i);

                    g.addEdge(i, i + 2);
                    g.addEdge(i + 2, i);

                    g.addEdge(i + 1, i + 3);
                    g.addEdge(i + 3, i + 1);

                    g.addEdge(i + 2, i + 3);
                    g.addEdge(i + 3, i + 2);

                    g[i].data = WorkGroup{
                        i,
                        { [&output, &outputMutex, i]() {
                            std::lock_guard<std::mutex> lk(outputMutex);
                            output.push_back(i);
                        } }
                    };

                    g[i + 1].data = WorkGroup{
                        i + 1,
                        { [&output, &outputMutex, i]() {
                            std::lock_guard<std::mutex> lk(outputMutex);
                            output.push_back(i + 1);
                        } }
                    };

                    g[i + 2].data = WorkGroup{
                        i + 1,
                        { [&output, &outputMutex, i]() {
                            std::lock_guard<std::mutex> lk(outputMutex);
                            output.push_back(i + 1);
                        } }
                    };

                    g[i + 3].data = WorkGroup{
                        i + 3,
                        { [&output, &outputMutex, i]() {
                            std::lock_guard<std::mutex> lk(outputMutex);
                            output.push_back(i + 3);
                        } }
                    };
                }

                ts.pushGraph(g);

                ts.helpWorkers();

                REQUIRE(output.size() == 1003);

                bool isSuccess = true;
                int prev = -1;
                for (auto i : output)
                {
                    if (i != prev + 1 && i != prev + 2 && i != prev)
                        isSuccess = false;

                    prev = i;
                }

                REQUIRE(isSuccess == true);
            }

            WHEN("The WorkGraph does have dependencies")
            {
                Task dep{};
        		for (auto i = 1u; i < 1000; i += 4)
        		{
        			WorkGraph g;

        			g.addEdge(0, 1);
        			g.addEdge(1, 0);

        			g.addEdge(0, 2);
        			g.addEdge(2, 0);

        			g.addEdge(1, 3);
        			g.addEdge(3, 1);

        			g.addEdge(2, 3);
        			g.addEdge(3, 2);

        			g[0].data = WorkGroup{
        				i,
        				{ [i, &output, &outputMutex]() {
        					std::lock_guard<std::mutex> lk(outputMutex);
        					output.push_back(i);
        				}, []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} }
        			};

        			g[1].data = WorkGroup{
        				i + 1,
        				{ [i, &output, &outputMutex]() {
        					std::lock_guard<std::mutex> lk(outputMutex);
        					output.push_back(i + 1);
        				}, []() {}, []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} }
        			};

        			g[2].data = WorkGroup{
        				i + 1,
        				{ [i, &output, &outputMutex]() {
        					std::lock_guard<std::mutex> lk(outputMutex);
        					output.push_back(i + 2);
        				}, []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} }
        			};

        			g[3].data = WorkGroup{
        				i + 3,
        				{ [i, &output, &outputMutex]() {
        					std::lock_guard<std::mutex> lk(outputMutex);
        					output.push_back(i + 3);
        				}, []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} , []() {} }
        			};

        			g[3].state = State::GREEN;

        			dep = ts.pushGraph(g, dep);
        		}

        		ts.helpWorkers();

                REQUIRE(output.size() == 1000);

                bool isSuccess = true;
                int pp = -1;
            	int p = 0;
            	for (auto i : output)
            	{
            		if (!((i == p + 1 && i == pp + 2) || (i == p - 1 && i == pp + 1) || (i == p + 2 && i == pp + 3) || (i == p + 2 && i == pp + 1) || (i==p+1 && i == pp+3)))
            			isSuccess = false;

            		pp = p;
            		p = i;
            	}

                REQUIRE(isSuccess == true);
            }
        }
    }
}
