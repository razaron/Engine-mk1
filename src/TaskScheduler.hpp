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
        *   A Task won't run until its dependency Task has finished. For a Task to be considered finished, it
        *   and all it's children must be finished.
        *
        *   @param  p_work          The work function to be called.
        *   @param  p_parent        <\em optional> The parent Task.
        *   @param  p_dependency    <\em optional> The dependency Task.
        *
        *   @return A copy of the created Task.
        */
        Task push(WorkFunc p_work, Task p_parent = Task{}, Task p_dependency = Task{});

        /*! Pushes a new Task to be scheduled.
        *   A Task won't run until its dependency Task has finished. For a Task to be considered finished, it
        *   and all it's children must be finished.
        *
        *   @param  p_work          The work function to be called.
        *   @param  p_parent        <\em optional> The parent Task.
        *   @param  p_dependency    <\em optional> The dependency Task.
        *
        *   @return A copy of the created Task.
        */
        Task pushGraph(WorkGraph p_workGraph, Task p_parent = Task{}, Task p_dependency = Task{});

        /*! A function to let the main thread help process Task%s until there are none left. */
        void helpWork();

      private:
        void worker();
        std::pair<TaskList::iterator, TaskList *> getParent(const Task &p_task);
        std::pair<TaskList::iterator, TaskList *> getDependency(const Task &p_task);
        Task getWork();

        std::atomic<std::size_t> m_nextTaskID{1};
        TaskList m_openTasks;
        TaskList m_pendingTasks;

        std::vector<std::thread> m_threads;
        std::mutex m_taskQueueMutex;
        std::condition_variable m_hasWorkCondition;

        bool m_isEnd = false;
        bool m_hasOpenWork = false;
        bool m_hasWork = false;
    };

    inline TaskScheduler::TaskScheduler()
    {
        for (auto i = 0u; i < std::thread::hardware_concurrency(); i++)
        {
            m_threads.emplace_back(&TaskScheduler::worker, this);
        }
    }

    inline TaskScheduler::~TaskScheduler()
    {
        {
            std::lock_guard<std::mutex> lk(m_taskQueueMutex);
            m_isEnd = true;
            m_hasOpenWork = true;
        }

        m_hasWorkCondition.notify_all();

        for (auto &t : m_threads)
        {
            t.join();
        }
    }

    inline Task TaskScheduler::push(WorkFunc p_work, Task p_parent, Task p_dependency)
    {
        std::lock_guard<std::mutex> lk(m_taskQueueMutex);

        // Create new task
        Task task{
            m_nextTaskID++,
            p_parent.taskID,
            p_dependency.taskID,
            p_work,
            1};

        //
        if (task.parentID)
        {
            auto pair = getParent(task);

            if (pair.first != pair.second->end())
                pair.first->openWorkItems++;
        }

        if (task.dependencyID)
        {
            auto pair = getDependency(task);

            if (pair.first != pair.second->end())
                m_pendingTasks.push_back(task);
            else
                m_pendingTasks.push_back(task);
        }
        else
            m_openTasks.push_back(task);

        m_hasWork = true;
        m_hasOpenWork = true;
        m_hasWorkCondition.notify_all();

        return task;
    }

    inline Task TaskScheduler::pushGraph(WorkGraph p_workGraph, Task p_parent, Task p_dependency)
    {
        std::list<Task> tasks;
        std::map<unsigned, Task> parents;

        p_workGraph.vertexFuncs[State::WHITE] = [this, &tasks, &parents](WorkGraphVertex &v, WorkGraph &g){
            unsigned depth = v.data.first;
            std::vector<WorkGraphVertex> dependencies;

            // Get all dependency work groups
            for(auto &e : v.adjacencyList)
            {
                if(g[e.target].data.first < depth)
                {
                    dependencies.push_back(g[e.target]);
                }
            }

            if(dependencies.size() == 0)
            {
                // Create a new parent task
                Task parent{
                    m_nextTaskID++,
                    0,
                    0,
                    v.data.second[0],
                    1
                };

                // Add it to the tasks list
                tasks.push_back(parent);

                // Add it to the parent map
                parents[v.id] = parent;

                // Add childen tasks to the task list
                auto w = v.data.second.begin();
                (w != v.data.second.end()) ? w++ : w;

                while( w != v.data.second.end())
                {
                    Task child{
                        m_nextTaskID++,
                        parent.taskID,
                        0,
                        *w,
                        1
                    };

                    tasks.push_back(child);

                    w++;
                }
            }
            else if(dependencies.size() == 1)
            {
                Task dep = parents[dependencies[0].id];

                // Create a new parent task
                Task parent{
                    m_nextTaskID++,
                    0,
                    dep.taskID,
                    v.data.second[0],
                    1
                };

                // Add it to the tasks list
                tasks.push_back(parent);

                // Add it to the parent map
                parents[v.id] = parent;

                // Add childen tasks to the task list
                auto w = v.data.second.begin();
                (w != v.data.second.end()) ? w++ : w;

                while( w != v.data.second.end())
                {
                    Task child{
                        m_nextTaskID++,
                        parent.taskID,
                        dep.taskID,
                        *w,
                        1
                    };

                    tasks.push_back(child);

                    w++;
                }
            }
            else
            {
                Task dummy{
                    m_nextTaskID++,
                    0,
                    0,
                    [v](){std::clog << "dummy for " << v.id << std::endl; },
                    1
                };

                // For each dependency, set its parent to the dummy
                for(auto &dep : dependencies)
                {
                    parents[dep.id].parentID = dummy.taskID;
                    dummy.openWorkItems++;
                }

                // Create a new parent task
                Task parent{
                    m_nextTaskID++,
                    0,
                    dummy.taskID,
                    v.data.second[0],
                    1
                };

                // Add it to the tasks list
                tasks.push_back(parent);

                // Add it to the parent map
                parents[v.id] = parent;

                // Add childen tasks to the task list
                auto w = v.data.second.begin();
                (w != v.data.second.end()) ? w++ : w;

                while( w != v.data.second.end())
                {
                    Task child{
                        m_nextTaskID++,
                        parent.taskID,
                        dummy.taskID,
                        *w,
                        1
                    };

                    tasks.push_back(child);

                    w++;
                }
            }
        };

        p_workGraph.breadthFirstTraversal(0);

        {
            std::lock_guard<std::mutex> lk(m_taskQueueMutex);

            if(m_pendingTasks.size())
                m_pendingTasks.splice(m_pendingTasks.end()--, tasks);
            else
                m_pendingTasks.splice(m_pendingTasks.begin(), tasks);

            m_hasOpenWork = true;
            m_hasWork = true;

            m_hasWorkCondition.notify_all();
        }

        return Task{};
    }

    inline void TaskScheduler::helpWork()
    {
        while (true)
        {
            //
            std::unique_lock<std::mutex> lk(m_taskQueueMutex);
            Task task = getWork();
            lk.unlock();

            // If work is available, do it
            if (task.work)
            {
                task.work();

                std::lock_guard<std::mutex> lg(m_taskQueueMutex);

                auto t = std::find(m_openTasks.begin(), m_openTasks.end(), task);
                t->openWorkItems--;

                // If task has no openWorkItems remaining
                if (t != m_openTasks.end() && !t->openWorkItems)
                {
                    // Remove task from open list
                    m_openTasks.erase(t);

                    // Decrement its parents openWorkItems count
                    auto pair = getParent(task);

                    if (pair.first != pair.second->end())
                    {
                        pair.first->openWorkItems--;

                        if (!pair.first->openWorkItems)
                        {
                            pair.second->erase(pair.first);
                        }
                    }

                    // Find if a dependant task is in the pending tasks list
                    auto pending = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(), [task](const Task &p) {
                        return p.dependencyID == task.taskID;
                    });

                    // Put the dependent task in the open queue
                    if (pending != m_pendingTasks.end())
                    {
                        m_openTasks.push_back(*pending);
                        m_pendingTasks.erase(pending);
                    }
                }

                m_hasOpenWork = m_pendingTasks.size() || m_openTasks.size();
                m_hasWork = m_pendingTasks.size() || m_openTasks.size();

                m_hasWorkCondition.notify_all();
            }
            // If no work is available
            else
            {
                bool isFinished = true;

                {
                    std::lock_guard<std::mutex> lk(m_taskQueueMutex);
                    isFinished = !m_pendingTasks.size() && !m_openTasks.size();
                }

                if (isFinished)
                    break;
                else
                {
                    std::unique_lock<std::mutex> lk(m_taskQueueMutex);
                    m_hasWorkCondition.wait(lk, [this] { return m_hasOpenWork || !m_hasWork; });
                    lk.unlock();
                }
            }
        }
    }

    inline std::pair<TaskList::iterator, TaskList *> TaskScheduler::getParent(const Task &p_task)
    {
        auto list = &m_openTasks;

        auto parent = std::find_if(m_openTasks.begin(), m_openTasks.end(), [task = p_task](const Task &t) {
            return t.taskID == task.parentID;
        });

        if (parent == m_openTasks.end())
        {
            list = &m_pendingTasks;

            parent = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(), [task = p_task](const Task &t) {
                return t.taskID == task.parentID;
            });
        }

        return std::make_pair(parent, list);
    }

    inline std::pair<TaskList::iterator, TaskList *> TaskScheduler::getDependency(const Task &p_task)
    {
        auto list = &m_openTasks;

        auto dependency = std::find_if(m_openTasks.begin(), m_openTasks.end(), [task = p_task](const Task &t) {
            return t.taskID == task.dependencyID;
        });

        if (dependency == m_openTasks.end())
        {
            list = &m_pendingTasks;

            dependency = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(), [task = p_task](const Task &t) {
                return t.taskID == task.dependencyID;
            });
        }

        return std::make_pair(dependency, list);
    }

    inline Task TaskScheduler::getWork()
    {
        if (m_openTasks.size())
        {
            // Makes a copy of the first task with work
            Task task{};
            for (auto &t : m_openTasks)
            {
                if (t.work)
                {
                    task = t;
                    break;
                }
            }

            // Modifies the original task in open list
            if (task.taskID)
            {
                auto t = std::find(m_openTasks.begin(), m_openTasks.end(), task);
                t->work = nullptr;

                // Returns the unmodified copy
                return task;
            }
        }

        if (m_pendingTasks.size())
        {
            // Find a pending task whose dependencies have been met
            Task task{};
            for (auto &t : m_pendingTasks)
            {
                auto pair = getDependency(t);

                if (pair.first == pair.second->end())
                {
                    task = t;
                    break;
                }
            }

            // Modifies the original task in open list
            if (task.taskID)
            {
                auto it = std::find(m_pendingTasks.begin(), m_pendingTasks.end(), task);
                it->work = nullptr;

                m_openTasks.push_back(*it);
                m_pendingTasks.erase(it);

                // Returns the unmodified copy
                return task;
            }
        }

        m_hasOpenWork = false;

        if (!m_pendingTasks.size() && !m_openTasks.size())
            m_hasWork = false;

        return Task{};
    }

    inline void TaskScheduler::worker()
    {
        while (!m_isEnd)
        {
            // Wait until tasks are available
            std::unique_lock<std::mutex> lk(m_taskQueueMutex);
            m_hasWorkCondition.wait(lk, [this] { return (m_hasOpenWork && m_hasWork) || m_isEnd; });

            // get task to work on
            Task task = getWork();

            // Unlock mutex, so other threads can get tasks
            lk.unlock();
            m_hasWorkCondition.notify_all();

            if (task.work)
            {
                task.work();

                std::lock_guard<std::mutex> lg(m_taskQueueMutex);

                auto t = std::find(m_openTasks.begin(), m_openTasks.end(), task);
                t->openWorkItems--;

                // If task has no openWorkItems remaining
                if (t != m_openTasks.end() && !t->openWorkItems)
                {
                    // Remove task from open list
                    m_openTasks.erase(t);

                    // Decrement its parents openWorkItems count
                    auto pair = getParent(task);

                    if (pair.first != pair.second->end())
                    {
                        pair.first->openWorkItems--;

                        if (!pair.first->openWorkItems)
                        {
                            pair.second->erase(pair.first);
                        }
                    }

                    // Find if a dependant task is in the pending tasks list
                    auto pending = std::find_if(m_pendingTasks.begin(), m_pendingTasks.end(), [task](const Task &p) {
                        return p.dependencyID == task.taskID;
                    });

                    // Put the dependent task in the open queue
                    if (pending != m_pendingTasks.end())
                    {
                        m_openTasks.push_back(*pending);
                        m_pendingTasks.erase(pending);
                    }
                }

                m_hasOpenWork = m_pendingTasks.size() || m_openTasks.size();
                m_hasWork = m_pendingTasks.size() || m_openTasks.size();

                m_hasWorkCondition.notify_all();
            }
        }
    }
}
