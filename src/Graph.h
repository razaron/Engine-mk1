#pragma once

#include <list>
#include <vector>
#include <algorithm>
#include <functional>

#define  VERTEX_WHITE 0x01 //Untouched
#define  VERTEX_GREY 0x02 //Touched
#define  VERTEX_BLACK 0x04 //Untouchable
#define  VERTEX_RED 0x08 //Failure
#define  VERTEX_GREEN 0x10 //Success

/*! Contains classes, enums and structs related to event streaming. */
namespace razaron::graph
{
	template <class, class> struct Vertex;
	template <class> struct Edge;
	template <class, class, class> class Graph;

	/*! A template struct for representing Vertex objects.
	*	
	*	@tparam V The data type to be held by the Vertex
	*	@tparam E The data type held by the Edge
	*/
	template <class V, class E>
	struct Vertex
	{
		V data;	/*! The data held by this Vertex */
		std::list<Edge<E>> adjacencyList; /*! An adjacency list of connected Edge%s*/
		unsigned short index; /*! An ID that doubles as an index value */
		char state; /*! The current state of the Vertex, represented by a bitfield */

		/*! Constructs an empty Vertex with index = p_index */
		Vertex(unsigned short p_index) :index(p_index), data(V{}), state(VERTEX_WHITE) {}
	};

	template <class E>
	struct Edge
	{
		E data;
		unsigned short source;
		unsigned short target;
	};

	template <class V, class E, class G>
	class Graph
	{
	public:
		Graph();
		Graph(Graph<V,E,G> &&other);
		~Graph();

		void breadthFirstSearch(unsigned short p_origin);
		void addEdge(E data, unsigned short source, unsigned short target);
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
	inline void Graph<V, E, G>::breadthFirstSearch(unsigned short p_origin)
	{
		if (!(*this)[p_origin].adjacencyList.size())
			return;

		//add this vertex to openQueue
		m_openQueue.push_back(&(*this)[p_origin]);

		//add adjacent vertices to openQueue
		for (auto& e : (*this)[p_origin].adjacencyList)
		{
			if (onEdgeDiscoverFunc)
				onEdgeDiscoverFunc(e, *this);

			m_openQueue.push_back(&(*this)[e.target]);
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
					if (v->state == VERTEX_WHITE)
					{
						if (onVertexDiscoverFunc)
							onVertexDiscoverFunc((*this)[v->index], *this);

						//add adjacent vertices to openQueue
						for (auto& e : (*this)[v->index].adjacencyList)
						{
							if (onEdgeDiscoverFunc)
								onEdgeDiscoverFunc(e, *this);

							m_openQueue.push_back(&(*this)[e.target]);
						}

						v->state = VERTEX_GREY;
					}

					return v->state != VERTEX_WHITE;
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

		(*this)[source].adjacencyList.push_back({data, source, target});
	}

	template<class V, class E, class G>
	inline void Graph<V, E, G>::reset()
	{
		for (auto& v : m_vertices)
		{
			v.state = VERTEX_WHITE;
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
		else if (m_vertices[p_index].index == p_index)
		{
			return m_vertices[p_index];
		}
		else
		{
			std::vector<Vertex<V, E>>::iterator it = std::find_if(m_vertices.begin(), m_vertices.end(),
				[p_index](const Vertex<V, E>& v) {return v.index == p_index; }
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


