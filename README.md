# engine-mk1
Click here for [documentation](https://razaron.github.io/engine-mk1/ "Documentation")
## TODO NON-CODE
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
- Hook [EnkiTS] into TaskScheduler

#### razaron::objectpool
- ObjectPool concurency v1
    - flat combining OR single shared lock for everything

### Code Branches
- example
    - barebones example showing preferred way to use space, systems, events processing etc.
- animal
    - the [animal](https://trello.com/b/tYrgDzDo/animal-v1 "Trello") game
