# engine-mk1
Click here for [documentation](https://razaron.github.io/engine-mk1/ "Documentation")
## TODO
- documentation
    - Component
    - Space
    - ObjectPool
    - ~EventStream~
    - System
    - Entity
    
- Make everything use ObjectPool
    - EventStream
    
- Miscelaneous code changes
    - Make EventStream use std::queue for m_incomingEvents and m_outgoingEvents
    - ObjectPool concurency v1
        - flat combining OR single shared lock for everything
- Make branches
    - example
        - barebones example showing preferred way to use space, systems, events processing etc.
    - animal
        - the [animal](https://trello.com/b/tYrgDzDo/animal-v1 "Trello") game
