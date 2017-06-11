#include "TaskScheduler.hpp"

using namespace razaron::taskscheduler;

TaskScheduler::TaskScheduler()
{
    for (auto i = 0u; i < std::thread::hardware_concurrency(); i++)
    {
        m_threads.emplace_back(&TaskScheduler::worker, this);
    }
}

TaskScheduler::~TaskScheduler()
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

Task TaskScheduler::push(WorkFunc p_work, Task p_dependency)
{
    std::lock_guard<std::mutex> lk(m_taskQueueMutex);

    // Create new task
    Task task{
        m_nextTaskID++,
        0,
        p_dependency.taskID,
        p_work,
        1 };

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

Task TaskScheduler::push(WorkGroup p_group, Task p_dependency)
{
    std::lock_guard<std::mutex> lk(m_taskQueueMutex);

    // Create new task
    Task parent{
        m_nextTaskID++,
        0,
        p_dependency.taskID,
        p_group.second[0],
        1 };

    if (parent.dependencyID)
    {
        auto pair = getDependency(parent);

        if (pair.first != pair.second->end())
            m_pendingTasks.push_back(parent);
        else
            m_pendingTasks.push_back(parent);
    }
    else
        m_openTasks.push_back(parent);

    auto work = p_group.second.begin();
    work++;
    while (work != p_group.second.end())
    {
        Task task{
            m_nextTaskID++,
            parent.taskID,
            p_dependency.taskID,
            p_group.second[0],
            1 };

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

        work++;
    }


    m_hasWork = true;
    m_hasOpenWork = true;
    m_hasWorkCondition.notify_all();

    return parent;
}

Task TaskScheduler::pushGraph(WorkGraph p_workGraph, Task p_dependency)
{
    std::list<Task> tasks;
    std::map<unsigned, Task> parents;
    Task dependency = p_dependency;
    p_workGraph.vertexFuncs[State::WHITE] = [this, &tasks, &parents, &dependency](WorkGraphVertex &v, WorkGraph &g){
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
				auto it = std::find(tasks.begin(), tasks.end(), parents[dep.id]);

				if (it != tasks.end())
				{
					it->parentID = dummy.taskID;
					dummy.openWorkItems++;
				}
            }

			tasks.push_back(dummy);

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

void TaskScheduler::helpWork()
{
	std::unique_lock<std::mutex> lk(m_taskQueueMutex);
	m_hasWorkCondition.wait(lk, [this] { return !m_hasWork; });
	lk.unlock();

	return;
	
	while (true)
    {
        Task task = getWork();

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

std::pair<TaskList::iterator, TaskList *> TaskScheduler::getParent(const Task &p_task)
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

std::pair<TaskList::iterator, TaskList *> TaskScheduler::getDependency(const Task &p_task)
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

Task TaskScheduler::getWork()
{
    std::lock_guard<std::mutex> lk(m_taskQueueMutex);

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

    m_hasWorkCondition.notify_all();

    return Task{};
}

void TaskScheduler::worker()
{
    while (!m_isEnd)
    {
        // Wait until tasks are available
        std::unique_lock<std::mutex> lk(m_taskQueueMutex);
        m_hasWorkCondition.wait(lk, [this] { return (m_hasOpenWork && m_hasWork) || m_isEnd; });
        lk.unlock();

        // get task to work on
        Task task = getWork();

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
