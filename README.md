# engine-mk1
Click here for [documentation](https://razaron.github.io/engine-mk1/ "Documentation")

## Compiling
#### Requirements
- gcc-7
- gnu make
- cmake
- python

#### Instructions
You can get a Windows binary for gcc-7 [here](http://www.equation.com/servlet/equation.cmd?fa=fortran)

Configure build.json by setting your include paths. Adding "includes", "build_opts", "compiler_opts" or "cmake_opts" to the "linux"/"windows" sections will override the defaults.

After configuring build.json, run build.py from the project directory.

## TODO
### Documentation
- Comment source (non-doxy)
- Flesh out doxy with guidelines
    - E.g. ObjectPool::getObject<T> T must be legit, else undefined
- Add [concepts](http://en.cppreference.com/w/cpp/concept "cppreference") for templates
- Add time complexity for algorithms

### Coding
- [Trello](https://trello.com/b/aAxYCKQL/engine-mk1)
