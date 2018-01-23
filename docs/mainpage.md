Main Page                         {#mainpage}
============

Engine MK1 is an Entity-Component based engine that stores data in ObjectPools and runs logic through a TaskScheduler.<br>
Storing data in an ObjectPool allows you to make better use of CPU cache, avoiding costly cache-misses and improving performance.<br>
Use of the TaskScheduler is optional. So you can have a mix of concurrent and serial code, giving greater flexibility.

- [Core](@ref core): Classes and structs for implementing an entity-component engine.
- [Graph](@ref graph): Lets you make a directed graph (cyclic or acyclic) and run BFS and DFS on it.
- [ObjectPool](@ref objectpool): Lets you store and access objects in contiguous aligned memory.
- [Planner](@ref planner): A goal oriented action planner implementation.
- [TaskScheduler](@ref taskscheduler): Takes a group of tasks (with option interdependencies) and schedules them to run on worker threads.
