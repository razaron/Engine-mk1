#pragma once

#include <list>
#include <vector>
#include <algorithm>
#include <functional>


/*! Contains classes, enums and structs related to event streaming. */
namespace razaron::graph
{
	template <class, class> struct Vertex;
	template <class> struct Edge;
	template <class, class, class> class Graph;

	const char STATE_WHITE = 0x01;	/*!< Untouched */
	const char STATE_GREY = 0x02;	/*!< Touched */
	const char STATE_BLACK = 0x04;	/*!< Untouchable */
	const char STATE_RED = 0x08;	/*!< Failure */
	const char STATE_GREEN = 0x10;	/*!< Success */

	/*! A template struct for representing Vertex objects.
	*	
	*	@tparam V The data type held by Vertex objects
	*	@tparam E The data type held by Edge objects
	*/
	template <class V, class E>
	struct Vertex
	{
		V data;								/*!< The data held by this Vertex. */
		std::list<Edge<E>> adjacencyList;	/*!< An adjacency list of connected Edge%s. */
		unsigned short id;					/*!< An ID that doubles as an index value. */
		char state;							/*!< The current state of the Vertex, represented by a bitfield. */

		/*! Constructs an empty Vertex with <tt>index == p_index</tt>. */
		Vertex(unsigned short p_index) :id(p_index), data(V{}), state(STATE_WHITE) {}
	};

	/*! A template struct for representing Edge objects.
	*
	*	@tparam E The data type held by the Edge
	*/
	template <class E>
	struct Edge
	{
		E data;					/*!< The data held by this Edge. */
		unsigned short source;	/*!< The id of the source Vertex. */
		unsigned short target;	/*!< The id of the target Vertex. */
		char state;				/*!< The current state of the Edge, represented by a bitfield. */
	};

	/*! A template struct for representing Edge objects.
	*
	*	@tparam V The data type held by Vertex objects
	*	@tparam E The data type held by Edge objects
	*	@tparam G The data type held by Graph objects
	*/
	template <class V, class E, class G>
	class Graph
	{
	public:
		Graph();						/*!< Default constructor. */
		Graph(Graph<V,E,G> &&other);	/*!< Move constructor. */
		~Graph();						/*!< Default destructor. */

		/*! Performs a breadth first traversal of the Graph.
		*	
		*	Starting at the Vertex with <tt>id == p_origin</tt>, traverses the Graph in breadth first order.
		*	Runs <tt>onVertexDiscoverFunc</tt> for every Vertex with <tt>state == STATE_WHITE</tt> and
		*	<tt>onEdgeDiscoverFunc</tt> for every Edge with <tt>state == STATE_WHITE</tt>.
		*
		*	By default Sets the <tt>state</tt> of every touched Vertex and Edge to <tt>STATE_GREY</tt> unless specified otherwise
		*	in <tt>onVertexDiscoverFunc</tt> or <tt>onEdgeDiscoverFunc</tt>. 
		*
		*	@param p_origin The <tt>id</tt> of the vertex to start the traversal from.
		*/
		void breadthFirstTraversal(unsigned short p_origin);

		/*!	Constructs and adds a new Edge to the Graph.
		*	
		*	If no Vertex exists with id p_source or p_target, constructs and adds new Vertex objects for the missing id%s to the Graph.
		*
		*	@param p_data The data held by the edge.
		*	@param p_source The id of the source Vertex.
		*	@param p_target The id of the target Vertex.
		*/
		void addEdge(E p_data, unsigned short p_source, unsigned short p_target);
		void reset();

		Vertex<V, E>& operator [](unsigned short p_index);
			
		G data;
		std::function<void(Vertex<V, E>&, Graph<V, E, G>&)> onVertexDiscoverFunc = nullptr;
		std::function<void(Edge<E>&, Graph<V, E, G>&)> onEdgeDiscoverFunc = nullptr;

	private:
		std::vector<Vertex<V, E>> m_vertices;
		std::list<Vertex<V, E>*> m_openQueue;
		std::list<Vertex<V, E>*> m_orderedQueue;
			
	};

	template<class V, class E, class G>
	inline Graph<V, E, G>::Graph()
	{
		g_constructed++;
	}

	template<class V, class E, class G>
	inline Graph<V, E, G>::Graph(Graph<V, E, G>&& other)
		:m_vertices(std::move(other.m_vertices)), m_openQueue(std::move(other.m_openQueue)), m_orderedQueue(std::move(other.m_orderedQueue))
	{
		g_constructed++;
	}

	template<class V, class E, class G>
	inline Graph<V, E, G>::~Graph()
	{
		g_destroyed++;
	}

	template<class V, class E, class G>
	inline void Graph<V, E, G>::breadthFirstTraversal(unsigned short p_origin)
	{
		if (!(*this)[p_origin].adjacencyList.size())
			return;

		//add this vertex to openQueue
		m_openQueue.push_back(&(*this)[p_origin]);

		//add adjacent vertices to openQueue
		for (auto& e : static_cast<Vertex<V,E>>((*this)[p_origin]).adjacencyList)
		{

			if (onEdgeDiscoverFunc && e.state == STATE_WHITE)
			{
				e.state = STATE_GREY;
				onEdgeDiscoverFunc(e, *this);
				m_openQueue.push_back(&(*this)[e.target]);
			}
			else if (e.state == STATE_WHITE)
			{
				e.state = STATE_GREY;
				m_openQueue.push_back(&(*this)[e.target]);
			}
		}

		auto curOpenQueue = m_openQueue;
		m_openQueue.clear();

		//Process open queue
		while (!curOpenQueue.empty())
		{
			curOpenQueue.erase(
				std::remove_if(curOpenQueue.begin(), curOpenQueue.end(), [&](Vertex<V, E>* v) 
				{
					//Process newly discovered vertex
					if (v->state == STATE_WHITE)
					{
						//sets vertex to touched (STATE_GREY)
						v->state = STATE_GREY;
						
						if (onVertexDiscoverFunc)
							onVertexDiscoverFunc((*this)[v->id], *this);

						//add adjacent vertices to openQueue
						for (auto& e : (*this)[v->id].adjacencyList)
						{
							if (onEdgeDiscoverFunc && e.state == STATE_WHITE)
							{
								e.state = STATE_GREY;
								onEdgeDiscoverFunc(e, *this);
								m_openQueue.push_back(&(*this)[e.target]);
							}
							else if (e.state == STATE_WHITE)
							{
								e.state = STATE_GREY;
								m_openQueue.push_back(&(*this)[e.target]);
							}
						}
					}

					return v->state != STATE_WHITE;
				})
			);
			
			std::clog << "curOpenQueue.size() = " << curOpenQueue.size() << std::endl;

			curOpenQueue = m_openQueue;
			m_openQueue.clear();
		}
	}

	template<class V, class E, class G>
	inline void Graph<V, E, G>::addEdge(E data, unsigned short source, unsigned short target)
	{
		try
		{
			(*this)[source];
		}
		catch(std::out_of_range e)
		{
			if (source >= m_vertices.size())
				m_vertices.emplace_back(source);
			else
				m_vertices.emplace(m_vertices.begin() + source, source);
		}

		try
		{
			(*this)[target];
		}
		catch (std::out_of_range e)
		{
			if(target >= m_vertices.size())
				m_vertices.emplace_back(target);
			else
				m_vertices.emplace(m_vertices.begin() + target, target);
		}

		(*this)[source].adjacencyList.push_back({data, source, target, STATE_WHITE});
	}

	template<class V, class E, class G>
	inline void Graph<V, E, G>::reset()
	{
		for (auto& v : m_vertices)
		{
			v.state = STATE_WHITE;
			
			for (auto& e : (*this)[v.id].adjacencyList)
			{
				e.state = STATE_WHITE;
			}
		}

		onEdgeDiscoverFunc = nullptr;
		onVertexDiscoverFunc = nullptr;
	}

	template<class V, class E, class G>
	inline Vertex<V, E>& graph::Graph<V, E, G>::operator[](unsigned short p_index)
	{
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
			std::vector<Vertex<V, E>>::iterator it = std::find_if(m_vertices.begin(), m_vertices.end(),
				[p_index](const Vertex<V, E>& v) {return v.id == p_index; }
			);

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


