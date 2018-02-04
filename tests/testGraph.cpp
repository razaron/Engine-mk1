#include <catch.hpp>

#include "Graph.hpp"
#include "Misc.hpp"

using namespace razaron::graph;

struct TestGraphData
{
    std::vector<int> v;
    int numVertices;
    int numEdges;
};

typedef Graph<int, const char *, TestGraphData> TestGraph;
typedef Vertex<int, const char *> TestVertex;
typedef Edge<const char *> TestEdge;

SCENARIO("Graphs can be traversed in several ways", "[graph]")
{
    GIVEN("The Graph {{0,1}, {0,2}, {1,3}, {2,3}, {3,4}}")
    {
        TestGraph g;

        g.addEdge(0, 0, "0");
        g.addEdge(0, 1, "0->1");
        g.addEdge(0, 2, "0->2");
        g.addEdge(1, 3, "1->3");
        g.addEdge(2, 3, "2->3");
        g.addEdge(3, 4, "3->4");

        g[0].data = 0;
        g[1].data = 1;
        g[2].data = 2;
        g[3].data = 3;
        g[4].data = 4;

        REQUIRE(g.order() == 5);

        WHEN("Performing depth first search")
        {
            g.vertexFuncs[State::WHITE] = [](TestVertex &v, TestGraph &g) {
                g.data.v.push_back(v.data);
            };

            g.breadthFirstTraversal(0);

            REQUIRE((g.data.v.size() == g.order() && g.order() == 5));
            REQUIRE((g.data.v[0] == 0 && g.data.v[1] == 1 && g.data.v[2] == 2 && g.data.v[3] == 3 && g.data.v[4] == 4));
        }
    }
}

SCENARIO("Graphs can call user defined functions on Vertex and Edge discovery", "[graph]")
{
    TestGraph g;

    g.addEdge(0, 0, "0");
    g.addEdge(0, 1, "0->1");
    g.addEdge(0, 2, "0->2");
    g.addEdge(1, 3, "1->3");
    g.addEdge(2, 3, "2->3");
    g.addEdge(3, 4, "3->4");

    g[0].data = 0;
    g[1].data = 1;
    g[2].data = 2;
    g[3].data = 3;
    g[4].data = 4;

    REQUIRE(g.order() == 5);

    GIVEN("The graph {{0,1}, {0,2}, {1,3}, {2,3}, {3,4}}")
    {
        WHEN("Passing discovery functions")
        {
            g.vertexFuncs[State::WHITE] = [](TestVertex &v, TestGraph &g) {
                UNUSED(v);

                g.data.numVertices++;
            };

            g.edgeFuncs[State::WHITE] = [](TestEdge &v, TestGraph &g) {
                UNUSED(v);

                g.data.numEdges++;
            };

            g.breadthFirstTraversal(0);

            REQUIRE(g.data.numEdges == 6);
            REQUIRE(g.data.numVertices == 5);
        }
    }
}
