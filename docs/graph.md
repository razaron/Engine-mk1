Graph                         {#graph}
=============
The Graph library is used to create directed graphs (cyclic or acyclic) and run various traversal algorithms on them.

## Examples

### Setting Up
It's generally a good idea to define some commonly used types and aliases beforehand.

```{cpp}
// Define custom data types
struct VertexData {char name; int value;};
using V = VertexData;
using E = const char *;
using G = int;

// Create aliases to desired Graph elements //TODO better word for elements
using MyGraph = rz::graph::Graph<V, E, G>;
using MyVertex = rz::graph::Vertex<V, E>;  
using MyEdge = rz::graph::Edge<E>;
```

### Creating A New Graph
To create a graph, you have to first define some Edge%s. Vertex objects are default initialized with appropriate id%s as you define edges.

```{cpp}
// Initialize and create new Graph
MyGraph g;

// Create Edges and set MyEdge::data
g.addEdge(0, 1, "Origin->A");
g.addEdge(0, 2, "Origin->B");
g.addEdge(1, 3, "A->C");
g.addEdge(1, 4, "A->D");
g.addEdge(2, 5, "B->E");
g.addEdge(3, 6, "C->F");
g.addEdge(4, 6, "D->F");
g.addEdge(4, 7, "D->G");
g.addEdge(5, 9, "E->I");
g.addEdge(6, 8, "F->H");
g.addEdge(7, 8, "G->H");
g.addEdge(9, 8, "I->H");

// Set MyVertex::data
g[0].data = {'Z', 0};
g[1].data = {'A', 0};
g[2].data = {'B', 0};
g[3].data = {'C', 0};
g[4].data = {'D', 0};
g[5].data = {'E', 0};
g[6].data = {'F', 0};
g[7].data = {'G', 0};
g[8].data = {'H', 0};
g[9].data = {'I', 0};

// Set MyGraph::data
g.data = 0;
```

\dotfile graph_initial.dot "Generated Graph"

### Running Traversal Algorithms
Running a traversal algorithm requires vertices to be in State::WHITE, which is how the library represents "untouched" vertices.

```{cpp}
// Define Vertex discovery function
g.vertexFuncs[State::WHITE] = [](MyVertex &v, MyGraph &g){
    std::cout << v.data.name << std::endl;
};

// Run a breadth first traversal calling the above defined lambda on every Vertex
g.breadthFirstTraversal(0);

// Reset all values to 0
g.reset(false); // true resets discovery functions, false leaves them alone
g.data = 0;
for(auto i=0u;i<10;i++)
    g[i].data.value = 0;

// Run a depth first traversal calling the above defined lambda on every Vertex
g.depthFirstTraversal(0);

// Reset all values to 0
g.reset(false); // true resets discovery functions, false leaves them alone
g.data = 0;
for(auto i=0u;i<10;i++)
    g[i].data.value = 0;

// Run a branch by branch traversal calling the above defined lambda on every Vertex
g.branchTraversal(0);
```

### Saving Graphs To Disc
Generated Graph%s can be saved to DOT files with custom (DOT) attributes (as seen above).

```{cpp}
// Define lambda for adding Vertex attributes to generated DOT graphs
const auto vAttr = [](const MyVertex &v) {
    std::stringstream attributes;

    attributes << v.id << "[label=\"" << v.data.name << ":" << v.data.value << "\"]";

    return attributes.str();
};

// Define lambda for adding Edge attributes to generated DOT graphs
const auto eAttr = [](const MyEdge &e) {
    std::stringstream attributes;

    attributes << "[labeltooltip=\"" << e.data << "\"]";

    return attributes.str();
};

// Run a depth first search, calling discovery functions as appropriate
g.vertexFuncs[State::WHITE] = [](MyVertex &v, MyGraph &g){
    std::cout << v.data.name << std::endl;
};

g.depthFirstTraversal(0);

// Get DOT string and save to file
std::ofstream dotFile("graph.dot");
auto dotStr = g.getDOT(vAttr, eAttr);
dotFile << dotStr;

```

\dotfile graph_dfs.dot "Depth First Traversal"
