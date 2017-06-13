#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "Graph.hpp"

/*! %Task schedulers enable easy concurrency by packaging logic into tasks that can be run in an ordered manner. */
namespace razaron::taskscheduler
{
    using namespace razaron::graph;

    struct Task;
    struct WorkGraphData;

    using TaskList = std::list<Task>;                             /*!< . */
    using WorkFunc = std::function<void()>;                       /*!< . */
    using WorkGroup = std::pair<unsigned, std::vector<WorkFunc>>; /*!< . */
    using WorkGraph = Graph<WorkGroup, char, WorkGraphData>;      /*!< . */
    using WorkGraphVertex = Vertex<WorkGroup, char>;              /*!< . */
    using WorkGraphEdge = Edge<char>;                             /*!< . */

    /*! The data to be held by the TaskGraph. */
    struct WorkGraphData
    {
    };

    /*! A data structure for representing a Task. */
    struct Task
    {
        std::size_t taskID;        /*!< The ID of this Task. */
        std::size_t parentID;      /*!< The parents Task ID. */
        std::size_t dependencyID;  /*!< The ID of the Task that must finish as a prerequisite. */
        WorkFunc work;             /*!< The work function to run. */
        std::size_t openWorkItems; /*!< The remaing work items equal to N+1 where N is the number of children Task%s. */

        /*! Basic equality comparator. */
        bool operator==(const Task &rhs) const
        {
            return taskID == rhs.taskID;
        }
    };

    /*! Creates and manages worker threads that work on dependency based Task lists.
    *   For more information and examples, see page \ref taskscheduler.
    */
    class TaskScheduler
    {
      public:
        TaskScheduler();  /*!< Default constructor. */
        ~TaskScheduler(); /*!< Default destructor. */

        /*! Pushes a new Task to be scheduled.
        *
        *   @param  p_work          The work function to be called.
        *   @param  p_dependency    <\em optional> The Task that must finish before this one can run.
        *
        *   @return A copy of the created Task.
        */
        Task push(WorkFunc p_work, Task p_dependency = Task{});

        Task push(WorkGroup p_group, Task p_dependency = Task{});

        /*! Creates a series of Task%s from a WorkGraph.
        *
        *   @param  p_workGraph     The Graph of WorkFunc%s to convert into Task%s.
        *   @param  p_dependency    <\em optional> The Task that must finish before this one can run.
        *
        *   @return The task generated from the WorkGroup with `state == State::GREEN`.
        */
        Task pushGraph(WorkGraph p_workGraph, Task p_dependency = Task{});

        /*! A function to let the main thread help process Task%s until there are none left. */
        void helpWorkers();

      private:
        void worker();
        std::pair<TaskList::iterator, TaskList *> getParent(const Task &p_task);
        std::pair<TaskList::iterator, TaskList *> getDependency(const Task &p_task);
        Task getTask();
        void doTask(Task task);

        std::atomic<std::size_t> m_nextTaskID{1};
        TaskList m_openTasks;
        TaskList m_pendingTasks;

        std::vector<std::thread> m_threads;
        std::mutex m_taskQueueMutex;
        std::condition_variable m_hasWorkCondition;

		std::atomic<bool> m_isEnd{false};
		std::atomic<bool> m_hasOpenWork{false};
		std::atomic<bool> m_hasWork{false};
    };
}
