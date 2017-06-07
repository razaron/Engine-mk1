#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
#include <map>

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
        Vertex<V,E> *parent;
        std::list<Edge<E>> adjacencyList; /*!< An std::list of connected Edge%s. */
        unsigned short id;                /*!< An ID that doubles as an index value. */
        State state;;                       /*!< The current state of the Vertex, represented by a bitfield. */

        /*! Constructs an empty Vertex with the ID <tt>p_index</tt>. */
        Vertex(unsigned short p_index) : data(V{}), parent(nullptr), id(p_index), state(State::WHITE) {}
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
        State state;            /*!< The current state of the Edge, represented by a bitfield. */
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
        Graph();                     /*!< Default constructor. */
        ~Graph(){}                  /*!< Default destructor. */

        /*!	Performs a breadth first traversal of the Graph.
		*
		*	Starting at the Vertex with the ID <tt>p_origin</tt>, traverses the Graph in breadth first order.
		*	Runs onVertexDiscoverFunc for every Vertex with the state <tt>State::WHITE</tt> and
		*	onEdgeDiscoverFunc for every Edge with the state <tt>State::WHITE</tt>.
		*
		*	By default Sets the state of every touched Vertex and Edge to <tt>State::GREY</tt> unless specified otherwise
		*	in onVertexDiscoverFunc or onEdgeDiscoverFunc.
		*
		*	@param	p_origin	The ID of the Vertex to start the traversal from.
		*/
        void breadthFirstTraversal(unsigned short p_origin);

        /*!	Constructs and adds a new Edge to the Graph.
		*
		*	If no Vertex objects exist with the IDs <tt>p_source</tt> or <tt>p_target</tt>, constructs and adds new Vertex objects for the missing IDs to the Graph.
		*
		*	@param	p_data		The data held by the edge.
		*	@param	p_source	The ID of the source Vertex.
		*	@param	p_target	The ID of the target Vertex.
		*/
        void addEdge(unsigned short source, unsigned short target, E data = E{});

        /*! Resets the state of all Edge and Vertex objects belonging to the Graph to State::WHITE. */
        void reset();

        /*! Returns the number of Vertex objects. */
        std::size_t size() { return m_vertices.size(); }

        /*!	Searched the graph for the Vertex with id == p_index.
		*
		*	@exception	std::out_of_range	p_index out of range for m_vertices.
		*
		*	@return	A reference to Vertex object.
		*/
        Vertex<V, E> &operator[](unsigned short p_index);

        G data{};                                                                             /*!< The data held by this Graph */
        std::map<State, VertexFunc> vertexFuncs; /*!< The function to run on Vertex discovery. */
        std::map<State, EdgeFunc> edgeFuncs;       /*<! The function to run on Edge discovery */

      private:
        std::vector<Vertex<V, E>> m_vertices;
        std::list<Vertex<V, E> *> m_openQueue;
        std::list<Vertex<V, E> *> m_orderedQueue;
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
            {State::GREEN, nullptr}
        };

        edgeFuncs = {
            {State::WHITE, nullptr},
            {State::GREY, nullptr},
            {State::BLACK, nullptr},
            {State::RED, nullptr},
            {State::GREEN, nullptr}
        };
    }

    template <class V, class E, class G>
    inline void Graph<V, E, G>::breadthFirstTraversal(unsigned short p_origin)
    {
        if (!(*this)[p_origin].adjacencyList.size())
            return;

        //add this vertex to openQueue
        m_openQueue.push_back(&(*this)[p_origin]);

        //add adjacent vertices to openQueue
        for (auto &e : (*this)[p_origin].adjacencyList)
        {
            if (e.state == State::WHITE)
            {
                e.state = State::GREY;

                if(edgeFuncs[State::WHITE])
                    edgeFuncs[State::WHITE](e, *this);

                m_openQueue.push_back(&(*this)[e.target]);
            }
        }

        auto curOpenQueue = m_openQueue;
        m_openQueue.clear();

        //Process open queue
        while (!curOpenQueue.empty())
        {
            curOpenQueue.erase(
                std::remove_if(curOpenQueue.begin(), curOpenQueue.end(), [&](Vertex<V, E> *v) {
                    //Process newly discovered vertex
                    if (v->state == State::WHITE)
                    {
                        //sets vertex to touched (State::GREY)
                        v->state = State::GREY;

                        if (vertexFuncs[State::WHITE])
                            vertexFuncs[State::WHITE]((*this)[v->id], *this);

                        //add adjacent vertices to openQueue
                        for (auto &e : (*this)[v->id].adjacencyList)
                        {
                            if (e.state == State::WHITE)
                            {
                                e.state = State::GREY;

                                if(edgeFuncs[State::WHITE])
                                    edgeFuncs[State::WHITE](e, *this);

                                m_openQueue.push_back(&(*this)[e.target]);
                            }
                        }
                    }

                    return v->state != State::WHITE;
                }));

            std::clog << "curOpenQueue.size() = " << curOpenQueue.size() << std::endl;

            curOpenQueue = m_openQueue;
            m_openQueue.clear();
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
        (*this)[target].parent = &(*this)[source];
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
            {State::GREEN, nullptr}
        };

        edgeFuncs = {
            {State::WHITE, nullptr},
            {State::GREY, nullptr},
            {State::BLACK, nullptr},
            {State::RED, nullptr},
            {State::GREEN, nullptr}
        };
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
