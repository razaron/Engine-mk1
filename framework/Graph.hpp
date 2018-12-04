#ifndef RZ_FRAMEWORK_GRAPH_HPP
#define RZ_FRAMEWORK_GRAPH_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

/*! Contains classes, enums and structs related to graphs and graph traversal. */
namespace rz::graph
{
    template <class V, class E>
    struct Vertex;
    template <class E>
    struct Edge;
    template <class V, class E, class G>
    class Graph;

    enum class State
    {
        WHITE, /*!< Untouched */
        GREY,  /*!< Touched */
        BLACK, /*!< Untouchable */
        RED,   /*!< Failure */
        GREEN  /*!< Success */
    };

    /*!	A template struct for representing Vertex objects.
	*
	*	@tparam	V	The data type held by Vertex objects
	*	@tparam	E	The data type held by Edge objects
	*/
    template <class V, class E>
    struct Vertex
    {
        V data;                           /*!< The data held by this Vertex. */
        std::list<Edge<E>> adjacencyList; /*!< An std::list of connected Edge%s. */
        std::size_t id;                   /*!< An ID that doubles as an index value. */
        State state;
        ; /*!< The current state of the Vertex, represented by a bitfield. */

        /*! Constructs an empty Vertex with the ID `index`. */
        Vertex(std::size_t index) : data{}, adjacencyList{}, id{ index }, state{} {}

        /*! Basic equality comparator. */
        bool operator==(const Vertex<V, E> &rhs) const noexcept
        {
            return id == rhs.id;
        }
    };

    /*!	A template struct for representing Edge objects.
	*
	*	@tparam	E	The data type held by the Edge
	*/
    template <class E>
    struct Edge
    {
        E data;             /*!< The data held by this Edge. */
        std::size_t source; /*!< The ID of the source Vertex. */
        std::size_t target; /*!< The ID of the target Vertex. */
        State state;        /*!< The current state of the Edge, represented by a bitfield. */
        float cost;
    };

    /*!	A template class for representing Graph objects.
	*
	*	@tparam	V	The data type held by Vertex objects
	*	@tparam	E	The data type held by Edge objects
	*	@tparam	G	The data type held by Graph objects
	*/
    template <class V, class E, class G>
    class Graph
    {
        using VertexFunc = std::function<void(Vertex<V, E> &, Graph<V, E, G> &)>;
        using EdgeFunc = std::function<void(Edge<E> &, Graph<V, E, G> &)>;

      public:
        Graph(std::size_t prealloc = 0) noexcept; /*!< Default constructor. */

        /*!	Get's a reference to the Vertex with `id == index`.
		*
		*	@exception	std::out_of_range	index out of range for _vertices.
		*
		*	@return	A reference to Vertex object.
		*/
        Vertex<V, E> &operator[](std::size_t index);

        /*!	Performs a breadth first traversal of the Graph.
		*
		*	Starting at the Vertex with the ID `origin`, traverses the Graph in breadth first order.
		*	Runs the defined discovery function for every Vertex and Edge.
		*
		*	By default, sets the state of every touched Vertex and Edge with `State::WHITE` to `State::GREY`
		*   unless specified otherwise in their discovery functions.
		*
		*	@param	origin	The ID of the Vertex to start the traversal from.
		*/
        void breadthFirstTraversal(std::size_t origin);

        /*!	Performs a depth first traversal of the Graph.
		*
		*	Starting at the Vertex with the ID `origin`, traverses the Graph in depth first order. Never repeats
		*	visited nodes, so works for cyclic graphs.
		*	Runs the defined discovery function for every Vertex and Edge.
		*
		*	By default, sets the state of every touched Vertex and Edge with `State::WHITE` to `State::GREY`
		*   unless specified otherwise in their discovery functions.
		*
		*	@param	origin	The ID of the Vertex to start the traversal from.
		*/
        void depthFirstTraversal(std::size_t origin);

        /*!	Performs a branch-by-branch traversal of the Graph.
		*
		*	Starting at the Vertex with the ID `origin`, traverses the Graph in depth first order. However, instead
		*	of ignoring visited nodes, it retraces its steps from the origin to the last unvisited node.
		*	Runs the defined discovery function for every Vertex and Edge.
		*
		*	Sets the state of every Vertex and Edge that won't be visited again to `State::GREY`
		*   unless specified otherwise in their discovery functions.
		*
		*	@param	origin	The ID of the Vertex to start the traversal from.
		*/
        void branchTraversal(std::size_t origin);

        /*!	Constructs and adds a new Edge to the Graph.
		*
		*	If no Vertex objects exist with the IDs `source` or `target`, constructs and adds new Vertex
		*   objects for the missing IDs to the Graph.
		*
		*	@param	edgeData	The data held by the edge.
		*	@param	source		The ID of the source Vertex.
		*	@param	target		The ID of the target Vertex.
		*/
        void addEdge(std::size_t source, std::size_t target, E edgeData = E{});

        /*! Resets the state of all Edge and Vertex objects belonging to the Graph to `State::WHITE`. */
        void reset();

        /*! Returns the number of Vertex objects. */
        std::size_t order() { return _vertices.size(); }

        std::size_t size();

        /*!	Converts the graph into a string representing a directed graph in the DOT language.
		*	Unless otherwise specified: Styling is default, vertices are labeled with their ID and edges have no label.

		*	@param	vertexAttributes	A functor to add tags to the vertices. Must return in the format `[tag=value tag=value]`
		*	@param	edgeAttributes		A functor to add tags to the edges. Must return in the format `[tag=value tag=value]`
		*
		*	@return	The graph converted into the DOT language.
		*/
        std::string getDOT(std::function<std::string(const Vertex<V, E> &)> vertexAttributes = nullptr, std::function<std::string(const Edge<E> &)> edgeAttributes = nullptr);

        G data{};                                /*!< The data held by this Graph */
        std::map<State, VertexFunc> vertexFuncs; /*!< The function to run on Vertex discovery. */
        std::map<State, EdgeFunc> edgeFuncs;     /*<! The function to run on Edge discovery */

      private:
        void branchHelper(std::size_t origin);

        std::vector<Vertex<V, E>> _vertices;
    };

    // Default constructor
    template <class V, class E, class G>
    inline Graph<V, E, G>::Graph(std::size_t prealloc) noexcept
        : _vertices{}
    {
        vertexFuncs = {
            { State::WHITE, nullptr },
            { State::GREY, nullptr },
            { State::BLACK, nullptr },
            { State::RED, nullptr },
            { State::GREEN, nullptr }
        };

        edgeFuncs = {
            { State::WHITE, nullptr },
            { State::GREY, nullptr },
            { State::BLACK, nullptr },
            { State::RED, nullptr },
            { State::GREEN, nullptr }
        };

        if (prealloc)
            _vertices.reserve(prealloc);
    }

    template <class V, class E, class G>
    inline Vertex<V, E> &graph::Graph<V, E, G>::operator[](std::size_t index)
    {
        if (index < _vertices.size() && _vertices[index].id == index)
        {
            return _vertices[index];
        }
        else
        {
            auto it = std::find_if(_vertices.begin(), _vertices.end(), [index](const Vertex<V, E> &v) {
                return v.id == index;
            });

            if (it == _vertices.end())
            {
                throw std::out_of_range("index not found in _vertices");
            }
            else
            {
                return *it;
            }
        }
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::breadthFirstTraversal(std::size_t origin)
    {
        // Return if origin has no adjacent vertices
        if (!(*this)[origin].adjacencyList.size())
            return;

        std::list<Vertex<V, E>> openList;
        std::list<Vertex<V, E>> closedList;

        //add this vertex to openQueue
        openList.push_back((*this)[origin]);

        //Process open queue
        while (!openList.empty())
        {
            // Pop a vertex from the openList
            auto v = *openList.begin();
            openList.pop_front();

            // If the vertex is already in the closed list, go to the next loop
            if (std::find(closedList.begin(), closedList.end(), (*this)[v.id]) != closedList.end())
                continue;
            else
                closedList.push_back((*this)[v.id]);

            // Run the vertices discovery function
            if (vertexFuncs[(*this)[v.id].state])
                vertexFuncs[(*this)[v.id].state]((*this)[v.id], *this);

            if ((*this)[v.id].state == State::WHITE)
                (*this)[v.id].state = State::GREY;

            // Loop through adjacent edges
            for (auto &e : (*this)[v.id].adjacencyList)
            {
                // Run the edges discovery function
                if (edgeFuncs[e.state])
                    edgeFuncs[e.state](e, *this);

                if (e.state == State::WHITE)
                    e.state = State::GREY;

                openList.push_back((*this)[e.target]);
            }
        }
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::depthFirstTraversal(std::size_t origin)
    {
        auto v = (*this)[origin];

        // Exit if vertex already visited
        if ((*this)[v.id].state == State::GREY)
            return;

        // Run the vertices discovery function
        if (vertexFuncs[(*this)[v.id].state])
            vertexFuncs[(*this)[v.id].state]((*this)[v.id], *this);

        if ((*this)[v.id].state == State::WHITE)
            (*this)[v.id].state = State::GREY;

        // Loop through adjacent edges
        for (auto &e : (*this)[v.id].adjacencyList)
        {
            // Run the edges discovery function
            if (edgeFuncs[e.state])
                edgeFuncs[e.state](e, *this);

            if (e.state == State::WHITE)
                e.state = State::GREY;

            depthFirstTraversal(e.target);
        }
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::branchTraversal(std::size_t origin)
    {
        while ((*this)[origin].state == State::WHITE)
        {
            branchHelper(origin);
        }
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::addEdge(std::size_t source, std::size_t target, E edgeData)
    {
        auto it = std::find(_vertices.begin(), _vertices.end(), Vertex<V, E>{ source });

        if (it == _vertices.end())
        {
            if (source >= _vertices.size())
                _vertices.emplace_back(source);
            else
                _vertices.emplace(_vertices.begin() + source, source);
        }

        it = std::find(_vertices.begin(), _vertices.end(), Vertex<V, E>{ target });

        if (it == _vertices.end())
        {
            if (target >= _vertices.size())
                _vertices.emplace_back(target);
            else
                _vertices.emplace(_vertices.begin() + target, target);
        }

        (*this)[source].adjacencyList.push_back({ edgeData, source, target, State::WHITE });
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::reset()
    {
        for (auto &v : _vertices)
        {
            v.state = State::WHITE;

            auto adjList = (*this)[v.id].adjacencyList;

            for (auto &e : adjList)
            {
                e.state = State::WHITE;
            }
        }

        vertexFuncs = {
            { State::WHITE, nullptr },
            { State::GREY, nullptr },
            { State::BLACK, nullptr },
            { State::RED, nullptr },
            { State::GREEN, nullptr }
        };

        edgeFuncs = {
            { State::WHITE, nullptr },
            { State::GREY, nullptr },
            { State::BLACK, nullptr },
            { State::RED, nullptr },
            { State::GREEN, nullptr }
        };
    }

    template <class V, class E, class G>
    inline std::size_t Graph<V, E, G>::size()
    {
        std::list<Edge<E>> edges;

        for (const auto &v : _vertices)
        {
            for (const auto &e : v.adjacencyList)
            {
                edges.push_back(e);
            }
        }

        return edges.size();
    }

    template <class V, class E, class G>
    inline std::string Graph<V, E, G>::getDOT(std::function<std::string(const Vertex<V, E> &)> vertexAttributes, std::function<std::string(const Edge<E> &)> edgeAttributes)
    {
        std::list<Edge<E>> edges;

        for (const auto &v : _vertices)
        {
            for (const auto &e : v.adjacencyList)
                if (!(e.source == 0 && e.target == 0))
                    edges.push_back(e);
        }

        std::stringstream dot;
        dot << "digraph G {\n";

        for (const auto &e : edges)
        {
            dot << std::to_string(e.source) << " -> " << std::to_string(e.target);

            if (edgeAttributes)
            {
                dot << edgeAttributes(e);
            }
            else
                dot << ";\n";
        }

        if (vertexAttributes)
        {
            for (const auto &v : _vertices)
            {
                dot << vertexAttributes(v);
            }
        }

        dot << "}";

        return dot.str();
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::branchHelper(std::size_t origin)
    {
        // Run the vertices discovery function
        if (vertexFuncs[(*this)[origin].state])
            vertexFuncs[(*this)[origin].state]((*this)[origin], *this);

        // recurse on the first non-grey child vertex
        for (auto &e : (*this)[origin].adjacencyList)
        {
            if ((*this)[e.target].state != State::GREY)
            {
                // Run the edges discovery function
                if (edgeFuncs[e.state])
                    edgeFuncs[e.state](e, *this);

                branchHelper(e.target);
                break;
            }
        }

        bool unvisited = false;
        for (auto &e : (*this)[origin].adjacencyList)
        {
            if ((*this)[e.target].state != State::GREY)
                unvisited = true;
        }

        if (!unvisited)
            (*this)[origin].state = State::GREY;
    }
}

#endif //RZ_FRAMEWORK_GRAPH_HPP
