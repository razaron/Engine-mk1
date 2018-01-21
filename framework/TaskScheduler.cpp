#include "TaskScheduler.hpp"

using namespace razaron::taskscheduler;

TaskScheduler::TaskScheduler()
{
    for (auto i = 0u; i < std::thread::hardware_concurrency(); i++)
    {
        _threads.emplace_back(&TaskScheduler::worker, this);
    }
}

TaskScheduler::~TaskScheduler()
{
    {
        std::lock_guard<std::mutex> lk(_taskQueueMutex);
        _isEnd = true;
        _hasOpenWork = true;
    }

    _hasWorkCondition.notify_all();

    for (auto &t : _threads)
    {
        t.join();
    }
}

Task TaskScheduler::push(WorkFunc work, Task dependency)
{
    std::lock_guard<std::mutex> lk(_taskQueueMutex);

    // Create new task
    Task task{
        _nextTaskID++,
        0,
        dependency.taskID,
        work,
        1
    };

    if (task.dependencyID)
    {
        auto pair = getDependency(task);

        if (pair.first != pair.second->end())
            _pendingTasks.push_back(task);
        else
            _openTasks.push_back(task);
    }
    else
        _openTasks.push_back(task);

    _hasWork = true;
    _hasOpenWork = true;
    _hasWorkCondition.notify_all();

    return task;
}

Task TaskScheduler::push(WorkGroup group, Task dependency)
{
    std::lock_guard<std::mutex> lk(_taskQueueMutex);

    // Create new task
    Task parent{
        _nextTaskID++,
        0,
        dependency.taskID,
        group.second[0],
        1
    };

    if (parent.dependencyID)
    {
        auto pair = getDependency(parent);

        if (pair.first != pair.second->end())
            _pendingTasks.push_back(parent);
        else
            _openTasks.push_back(parent);
    }
    else
        _openTasks.push_back(parent);

    auto work = group.second.begin();
    work++;
    while (work != group.second.end())
    {
        Task task{
            _nextTaskID++,
            parent.taskID,
            dependency.taskID,
            group.second[0],
            1
        };

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
                _pendingTasks.push_back(task);
            else
                _openTasks.push_back(task);
        }
        else
            _openTasks.push_back(task);

        work++;
    }

    _hasWork = true;
    _hasOpenWork = true;
    _hasWorkCondition.notify_all();

    return parent;
}

Task TaskScheduler::pushGraph(WorkGraph workGraph, Task dependency)
{
    std::list<Task> tasks;
    std::map<unsigned, Task> parents;

    auto fillTaskList = [this, &tasks, &parents, &dependency](WorkGraphVertex &v, WorkGraph &g) {
        unsigned depth = v.data.first;
        std::vector<WorkGraphVertex> dependencies;

        // Get all dependency work groups
        for (auto &e : v.adjacencyList)
        {
            if (g[e.target].data.first < depth)
            {
                dependencies.push_back(g[e.target]);
            }
        }

        auto pushTasks = [this, &v, &tasks, &parents](std::size_t depID){
            // Create a new parent task
            Task parent{
                _nextTaskID++,
                0,
                depID,
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

            while (w != v.data.second.end())
            {
                Task child{
                    _nextTaskID++,
                    parent.taskID,
                    depID,
                    *w,
                    1
                };

                tasks.push_back(child);

				auto it = std::find(tasks.begin(), tasks.end(), parent);
				it->openWorkItems++;

                w++;
            }

            return parent;
        };

        // Generate tasks dependent on the optional dependency
        if (dependencies.size() == 0)
        {
            return pushTasks(dependency.taskID);
        }
        // Generate tasks dependent on the sole dependency
        else if (dependencies.size() == 1)
        {
            Task dep = parents[dependencies[0].id];

            return pushTasks(dep.taskID);
        }
        // Insert a dummy task as the parent of the dependencies, then generate tasks dependent on the dummy
        else
        {
            Task dummy{
                _nextTaskID++,
                0,
                0,
                []() { },
                1
            };

            // For each dependency, set its parent to the dummy
            for (auto &dep : dependencies)
            {
                auto it = std::find(tasks.begin(), tasks.end(), parents[dep.id]);

                if (it != tasks.end())
                {
                    it->parentID = dummy.taskID;
                    dummy.openWorkItems++;
                }
            }

            tasks.push_back(dummy);

            return pushTasks(dummy.taskID);
        }
    };

    workGraph.vertexFuncs[State::WHITE] = [&fillTaskList](WorkGraphVertex &v, WorkGraph &g) {
        fillTaskList(v, g);
    };

    Task retDep{};

    workGraph.vertexFuncs[State::GREEN] = [&fillTaskList, &retDep](WorkGraphVertex &v, WorkGraph &g) {
        retDep = fillTaskList(v, g);
    };

    workGraph.breadthFirstTraversal(0);

    {
        std::lock_guard<std::mutex> lk(_taskQueueMutex);

        if (_pendingTasks.size())
            _pendingTasks.splice(_pendingTasks.end()--, tasks);
        else
            _pendingTasks.splice(_pendingTasks.begin(), tasks);

        _hasOpenWork = true;
        _hasWork = true;

        _hasWorkCondition.notify_all();
    }

    return retDep;
}

void TaskScheduler::helpWorkers()
{
	while (true)
    {
        Task task = getTask();

        // If work is available, do it
        if (task.work)
        {
            doTask(task);
        }
        // If no work is available
        else
        {
            bool isFinished = true;

            {
                std::lock_guard<std::mutex> lk(_taskQueueMutex);
                isFinished = !_pendingTasks.size() && !_openTasks.size();
            }

            if (isFinished)
                break;
            else
            {
                std::unique_lock<std::mutex> lk(_taskQueueMutex);
                _hasWorkCondition.wait(lk, [this] { return _hasOpenWork || !_hasWork; });
                lk.unlock();
            }
        }
    }
}

std::pair<TaskList::iterator, TaskList *> TaskScheduler::getParent(const Task &task)
{
    auto list = &_openTasks;

    auto parent = std::find_if(_openTasks.begin(), _openTasks.end(), [task = task](const Task &t) {
        return t.taskID == task.parentID;
    });

    if (parent == _openTasks.end())
    {
        list = &_pendingTasks;

        parent = std::find_if(_pendingTasks.begin(), _pendingTasks.end(), [task = task](const Task &t) {
            return t.taskID == task.parentID;
        });
    }

    return std::make_pair(parent, list);
}

std::pair<TaskList::iterator, TaskList *> TaskScheduler::getDependency(const Task &task)
{
    auto list = &_openTasks;

    auto dependency = std::find_if(_openTasks.begin(), _openTasks.end(), [task = task](const Task &t) {
        return t.taskID == task.dependencyID;
    });

    if (dependency == _openTasks.end())
    {
        list = &_pendingTasks;

        dependency = std::find_if(_pendingTasks.begin(), _pendingTasks.end(), [task = task](const Task &t) {
            return t.taskID == task.dependencyID;
        });
    }

    return std::make_pair(dependency, list);
}

Task TaskScheduler::getTask()
{
    std::lock_guard<std::mutex> lk(_taskQueueMutex);

    if (_openTasks.size())
    {
        // Makes a copy of the first task with work
        Task task{};
        for (auto &t : _openTasks)
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
            auto t = std::find(_openTasks.begin(), _openTasks.end(), task);
            t->work = nullptr;

            // Returns the unmodified copy
            return task;
        }
    }

    if (_pendingTasks.size())
    {
        // Find a pending task whose dependencies have been met
        Task task{};
        for (auto &t : _pendingTasks)
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
            auto it = std::find(_pendingTasks.begin(), _pendingTasks.end(), task);
            it->work = nullptr;

            _openTasks.push_back(*it);
            _pendingTasks.erase(it);

            // Returns the unmodified copy
            return task;
        }
    }

    _hasOpenWork = false;

    if (!_pendingTasks.size() && !_openTasks.size())
        _hasWork = false;

    _hasWorkCondition.notify_all();

    return Task{};
}

void TaskScheduler::doTask(Task task)
{
    task.work();

    std::lock_guard<std::mutex> lg(_taskQueueMutex);

    auto t = std::find(_openTasks.begin(), _openTasks.end(), task);
    t->openWorkItems--;

    // If task has no openWorkItems remaining
    if (t != _openTasks.end() && !t->openWorkItems)
    {
        // Remove task from open list
        _openTasks.erase(t);

		// Get all parents
        std::list<std::pair<TaskList::iterator, TaskList *>> parents;

		auto pair = getParent(task);
		while (pair.first != pair.second->end())
		{
			parents.push_back(pair);
			pair = getParent(*(parents.back().first));
		}

		// Successively decrements parents work items and erase if neccassary
		auto it = parents.begin();
		while (it != parents.end())
		{
			it->first->openWorkItems--;

			if (!it->first->openWorkItems)
			{
				it->second->erase(it->first);
				++it;
			}
			else
				it = parents.end();
		}

        // Find if a dependant task is in the pending tasks list
        auto pending = std::find_if(_pendingTasks.begin(), _pendingTasks.end(), [task](const Task &p) {
            return p.dependencyID == task.taskID;
        });

        // Put the dependent task in the open queue
        if (pending != _pendingTasks.end())
        {
            _openTasks.push_back(*pending);
            _pendingTasks.erase(pending);
        }
    }

    _hasOpenWork = _pendingTasks.size() || _openTasks.size();
    _hasWork = _pendingTasks.size() || _openTasks.size();

    _hasWorkCondition.notify_all();
}

void TaskScheduler::worker()
{
    while (!_isEnd)
    {
        // Wait until tasks are available
        std::unique_lock<std::mutex> lk(_taskQueueMutex);
        _hasWorkCondition.wait(lk, [this] { return (_hasOpenWork && _hasWork) || _isEnd; });
        lk.unlock();

        // get task to work on
        Task task = getTask();

        if (task.work)
        {
            doTask(task);
        }
    }
}
