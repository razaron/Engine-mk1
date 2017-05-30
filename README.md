# engine-mk1
Click here for [documentation](https://razaron.github.io/engine-mk1/ "Documentation")
## TODO
### Documentation
- Component
- Space
- ObjectPool
- ~EventStream~
- System
- Entity
- ~Graph~
- Add [concepts](http://en.cppreference.com/w/cpp/concept "cppreference") for templates
- Add time complexity for algorithms

### Code Updates
#### razaron::eventstream
- EventStream use std::queue for m_incomingEvents and m_outgoingEvents
- EventStream use ObjectPool for storing Event objects

#### razaron::render
- Hook [SIGIL](http://www.libsigil.com/) into RenderSystem

#### razaron::taskscheduler
- Hook [EnkiTS](https://github.com/dougbinks/enkiTS/tree/C++11) into TaskScheduler

#### razaron::objectpool
- Miscelaneous
    - getObject : Return a fail value
- ObjectPool concurency v1
    - [flat combining](http://people.csail.mit.edu/shanir/publications/Flat%20Combining%20SPAA%2010.pdf "pdf") OR [single shared lock](http://en.cppreference.com/w/cpp/thread/lock_guard "cppreference") for everything
- Add optional reference counting
```C++
  // Add bool countRef to relevant functions.
  // E.g,
  template<class T, class... Args> Handle emplace(Args... p_args, bool countRef);
  // then in function removeObject
  void removeObject(Handle p_handle){
    if(!hasReferences(p_handle)){
      // delete it
    }
  }
```

### Code Branches
- example
    - barebones example showing preferred way to use space, systems, events processing etc.
- animal
    - the [animal](https://trello.com/b/tYrgDzDo/animal-v1 "Trello") game
