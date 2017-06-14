#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>

/*! Contains classes, enums and structs related to graphs and graph traversal. */
namespace razaron::graph
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
        unsigned short id;                /*!< An ID that doubles as an index value. */
        State state;
        ; /*!< The current state of the Vertex, represented by a bitfield. */

        /*! Constructs an empty Vertex with the ID `p_index`. */
        Vertex(unsigned short p_index) : data(V{}), id(p_index), state(State::WHITE) {}

        /*! Basic equality comparator. */
        bool operator==(const Vertex<V,E> &rhs) const
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
        E data;                /*!< The data held by this Edge. */
        unsigned short source; /*!< The ID of the source Vertex. */
        unsigned short target; /*!< The ID of the target Vertex. */
        State state;           /*!< The current state of the Edge, represented by a bitfield. */
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
        Graph();    /*!< Default constructor. */
        ~Graph() {} /*!< Default destructor. */

        /*!	Performs a breadth first traversal of the Graph.
		*
		*	Starting at the Vertex with the ID `p_origin`, traverses the Graph in breadth first order.
		*	Runs the defined discovery function for every Vertex and Edge.
		*
		*	By default, sets the state of every touched Vertex and Edge with `State::WHITE` to `State::GREY`
        *   unless specified otherwise in their discovery functions.
		*
		*	@param	p_origin	The ID of the Vertex to start the traversal from.
		*/
        void breadthFirstTraversal(unsigned short p_origin);

        /*!	Constructs and adds a new Edge to the Graph.
		*
		*	If no Vertex objects exist with the IDs `p_source` or `p_target`, constructs and adds new Vertex
        *   objects for the missing IDs to the Graph.
		*
		*	@param	p_data		The data held by the edge.
		*	@param	p_source	The ID of the source Vertex.
		*	@param	p_target	The ID of the target Vertex.
		*/
        void addEdge(unsigned short source, unsigned short target, E data = E{});

        /*! Resets the state of all Edge and Vertex objects belonging to the Graph to `State::WHITE`. */
        void reset();

        /*! Returns the number of Vertex objects. */
        std::size_t size() { return m_vertices.size(); }

        /*!	Get's a reference to the Vertex with `id == p_index`.
		*
		*	@exception	std::out_of_range	p_index out of range for m_vertices.
		*
		*	@return	A reference to Vertex object.
		*/
        Vertex<V, E> &operator[](unsigned short p_index);

        G data{};                                /*!< The data held by this Graph */
        std::map<State, VertexFunc> vertexFuncs; /*!< The function to run on Vertex discovery. */
        std::map<State, EdgeFunc> edgeFuncs;     /*<! The function to run on Edge discovery */

      private:
        std::vector<Vertex<V, E>> m_vertices;
    };

    // Default constructor
    template <class V, class E, class G>
    inline Graph<V, E, G>::Graph()
    {
        vertexFuncs = {
            {State::WHITE, nullptr},
            {State::GREY, nullptr},
            {State::BLACK, nullptr},
            {State::RED, nullptr},
            {State::GREEN, nullptr}};

        edgeFuncs = {
            {State::WHITE, nullptr},
            {State::GREY, nullptr},
            {State::BLACK, nullptr},
            {State::RED, nullptr},
            {State::GREEN, nullptr}};
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::breadthFirstTraversal(unsigned short p_origin)
    {
        // Return if p_origin has no adjacent vertices
        if (!(*this)[p_origin].adjacencyList.size())
            return;

        std::list<Vertex<V, E>> openList;
        std::list<Vertex<V, E>> closedList;

        //add this vertex to openQueue
        openList.push_back((*this)[p_origin]);

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

            if((*this)[v.id].state == State::WHITE)
                (*this)[v.id].state = State::GREY;

            // Loop through adjacent edges
            for (auto &e : (*this)[v.id].adjacencyList)
            {
                // Run the edges discovery function
                if (edgeFuncs[e.state])
                    edgeFuncs[e.state](e, *this);

                if(e.state == State::WHITE)
                    e.state = State::GREY;

                openList.push_back((*this)[e.target]);
            }

            std::clog << "openList.size() = " << openList.size() << std::endl;
        }
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::addEdge(unsigned short source, unsigned short target, E data)
    {
        // look for vertex with ID source
        try
        {
            (*this)[source];
        }
        // if it's not found, construct it
        catch (std::out_of_range e)
        {
            if (source >= m_vertices.size())
                m_vertices.emplace_back(source);
            else
                m_vertices.emplace(m_vertices.begin() + source, source);
        }

        // look for vertex with ID target
        try
        {
            (*this)[target];
        }
        catch (std::out_of_range e)
        {
            if (target >= m_vertices.size())
                m_vertices.emplace_back(target);
            else
                m_vertices.emplace(m_vertices.begin() + target, target);
        }

        (*this)[source].adjacencyList.push_back({data, source, target, State::WHITE});
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::reset()
    {
        for (auto &v : m_vertices)
        {
            v.state = State::WHITE;

            for (auto &e : (*this)[v.id].adjacencyList)
            {
                e.state = State::WHITE;
            }
        }

        vertexFuncs = {
            {State::WHITE, nullptr},
            {State::GREY, nullptr},
            {State::BLACK, nullptr},
            {State::RED, nullptr},
            {State::GREEN, nullptr}};

        edgeFuncs = {
            {State::WHITE, nullptr},
            {State::GREY, nullptr},
            {State::BLACK, nullptr},
            {State::RED, nullptr},
            {State::GREEN, nullptr}};
    }

    template <class V, class E, class G>
    inline Vertex<V, E> &graph::Graph<V, E, G>::operator[](unsigned short p_index)
    {
        typedef typename std::vector<Vertex<V, E>>::iterator Iterator;

        if (p_index >= m_vertices.size())
        {
            throw std::out_of_range("p_index out of range for m_vertices");
        }
        else if (m_vertices[p_index].id == p_index)
        {
            return m_vertices[p_index];
        }
        else
        {
            auto lamda = [p_index](const Vertex<V, E> &v) { return v.id == p_index; };
            Iterator it = std::find_if(m_vertices.begin(), m_vertices.end(), lamda);

            if (it == m_vertices.end())
            {
                throw std::out_of_range("p_index not found in m_vertices");
            }
            else
            {
                return *it;
            }
        }
    }
}
