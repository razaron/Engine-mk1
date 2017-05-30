# engine-mk1
Click here for [documentation](https://razaron.github.io/engine-mk1/ "Documentation")
## TODO
### Documentation
- Space
- System
- ~~Entity~~
- ~~Component~~
- ~~ObjectPool~~
- ~~EventStream~~
- ~~Graph~~
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
- Miscellaneous
    - getObject : Return a fail value
- ObjectPool concurrency v1
    - [flat combining](http://people.csail.mit.edu/shanir/publications/Flat%20Combining%20SPAA%2010.pdf "pdf") OR [single shared lock](http://en.cppreference.com/w/cpp/thread/lock_guard "cppreference") for everything
- Add optional reference counting (for remove) via [std::shared_ptr](http://en.cppreference.com/w/cpp/memory/shared_ptr "cppreference")

### Miscellaneous
- Replace pointers with references for all function parameters and return types

### Unit Testing
- Check out [Catch](https://github.com/philsquared/Catch/blob/master/docs/tutorial.md "Catch Tutorial")

### Code Branches
- master
    - Brick breaker example
- clean
    - Barebones branch with derived systems/components removed.
    - Example showing preferred way to use space, systems, events processing etc.
- animal (??? separate project instead ???)
    - The [animal](https://trello.com/b/tYrgDzDo/animal-v1 "Trello") game
