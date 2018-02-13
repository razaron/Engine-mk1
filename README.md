# engine-mk1
Click here for [documentation](https://razaron.github.io/engine-mk1/ "Documentation")

## Compiling
### Instructions
#### Windows
- Visual Studio 15.6 Preview 4

That's all. 3rd party libraries and headers are already included.<br>
VS 15.4 lacks fold expressions and 15.5 has a compiler bug, hence the 15.6 preview version.

````bash
cd "location/of/engine-mk1"
mkdir ../build
cd ../build
cmake -G "Visual Studio 15 2017 Win64" ../engine-mk1/
````

In the build directory, there should be a `.sln` file. Open it in VS and build it.
#### Linux
- GCC 7.2
- libsfml-dev
- liblua5.3-dev

Just install the libs normally through apt.

````bash
cd "location/of/engine-mk1"
mkdir ../build
cd ../build
cmake -G "Unix Makefiles" ../engine-mk1/
make
````
## TODO
### Documentation
- Comment source (non-doxy)
- Flesh out doxy with guidelines
    - E.g. ObjectPool::getObject<T> T must be legit, else undefined
- Add [concepts](http://en.cppreference.com/w/cpp/concept "cppreference") for templates
- Add time complexity for algorithms

### Coding
- [Trello](https://trello.com/b/aAxYCKQL/engine-mk1)
