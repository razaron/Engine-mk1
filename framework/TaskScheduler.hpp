#pragma once

#include "Graph.hpp"

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
		std::size_t taskID{};        /*!< The ID of this Task. */
        std::size_t parentID{};      /*!< The parents Task ID. */
        std::size_t dependencyID{};  /*!< The ID of the Task that must finish as a prerequisite. */
        WorkFunc work{};             /*!< The work function to run. */
        std::size_t openWorkItems{}; /*!< The remaing work items equal to N+1 where N is the number of children Task%s. */

        /*! Basic equality comparator. */
        bool operator==(const Task &rhs) const noexcept
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
        TaskScheduler() noexcept;						/*!< Default constructor. */
		TaskScheduler(const TaskScheduler&) = delete;
		TaskScheduler(TaskScheduler&&) = delete;
		
        ~TaskScheduler();								/*!< Default destructor. */

		TaskScheduler& operator=(const TaskScheduler &) = delete;
		TaskScheduler& operator=(TaskScheduler &&) = delete;

        /*! Pushes a new Task to be scheduled.
        *
        *   @param  work          The work function to be called.
        *   @param  dependency    <\em optional> The Task that must finish before this one can run.
        *
        *   @return A copy of the created Task.
        */
        Task push(WorkFunc work, Task dependency = Task{});

        /*! Pushes a group Task to be scheduled.
        *   The first WorkFunc in the WorkGroup becomes the parent Task, with the remaining WorkFunc%s becoming
        *   child Task%s of the parent Task.
        *
        *   @param  group         The WorkGroup to be create Task%s from.
        *   @param  dependency    <\em optional> The Task that must finish before this one can run.
        *
        *   @return A copy of the parent Task.
        */
        Task push(WorkGroup group, Task dependency = Task{});

        /*! Creates and pushes a series of interdependent Task%s from a WorkGraph.
        *
        *   @param  workGraph     The Graph of WorkFunc%s to convert into Task%s.
        *   @param  dependency    <\em optional> The Task that must finish before this one can run.
        *
        *   @return The task generated from the WorkGroup with `state == State::GREEN`.
        */
        Task pushGraph(WorkGraph workGraph, Task dependency = Task{});

        /*! A function to let the main thread help process Task%s until there are none left. */
        void helpWorkers();

      private:
        void worker();
        std::pair<TaskList::iterator, TaskList *> getParent(const Task &task);
        std::pair<TaskList::iterator, TaskList *> getDependency(const Task &task);
        Task getTask();
        void doTask(Task task);

        std::atomic<std::size_t> _nextTaskID;
        TaskList _openTasks;
        TaskList _pendingTasks;

        std::vector<std::thread> _threads;
        std::mutex _taskQueueMutex;
        std::condition_variable _hasWorkCondition;

		std::atomic<bool> _isEnd;
		std::atomic<bool> _hasOpenWork;
		std::atomic<bool> _hasWork;
    };
}
