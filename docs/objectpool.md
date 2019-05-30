ObjectPool                         {#objectpool}
=============
The ObjectPool library is used store and access objects in contiguous aligned memory. Operations are performed throught the use of Handle%s.

## Examples

### Allocating to the pool

The ObjectPool has several methods of adding objects, all of which return Handle%s which can be used to later access said objects. 

```{cpp}
struct Data {int x; char y;}

// Initialize a new ObjectPool
rz::objectpool::ObjectPool p;

// In-place construction
auto h1 = p.emplace<Data>(1, 'a');

// Passing const reference
Data d{2, 'b'};
auto h2 = p.push(d);

// Passing rvalue
auto h3 = p.push(Data{3, 'c'});
```

### Accessing allocated objects

Accessing the underlying memory pointers is done through Handle%s.
As the ObjectPool doesn't store types, the desired objects type has to be supplied as a template argument (i.e. Data).

```{cpp}
auto fooPtr = p.get<Data>(h1);
std::cout << h1Ptr->x << ":" << h1Ptr->y << std::endl; // outputs "a:1"

auto barPtr = p.get<Data>(h2);
std::cout << h2Ptr->x << ":" << h2Ptr->y << std::endl; // outputs "b:2"

auto bazPtr = p.get<Data>(h3);
std::cout << h3Ptr->x << ":" << h3Ptr->y << std::endl; // outputs "c:3"
```

### Destroying allocated objects

Destroying allocated objects is also done through Handle%s.
In order for the ObjectPool to determine which underlying Pool the object is allocated in and call its destructor (for non-trivially destructable types), the objects type must be supplied as a template argument. 

```{cpp}
p.erase<Data>(h1);
p.erase<Data>(h2);
p.erase<Data>(h3);
```

### Making use of automatic lifetime tracking

The ObjectPool supports automatic lifetime tracking through the use of std::shared_ptr and std::unique_ptr.

```{cpp}
// Make a std::unique_ptr
auto unique = p.makeUnique<Data>(h1);

// Make a std::shared_ptr
auto shared = p.makeShared<Data>(h2);

// Access and modify underlying objects
auto data1 = p.get<Data>(*unique);
data1->x = 101;

auto data2 = p.get<Data>(*shared);
data2->x = 102;

// Destroy underlying objects. The destructor will be called for any non-trivially destructable object
unique = nullptr;
shared = nullptr;
```

### Utility functions

The ObjectPool can defragment itself in order to move disparate objects into the fewest contiguous memory regions possible.
Pointers are invalidated by this process, however, handles are not.

```{cpp}
p.defragment();
```

The ObjectPool can remove any unused Page%s in its underlying memory manager.
This is useful when a large number of identically sized objects are bulk allocated then erased, which would leave some unused Page%s behind. 

```{cpp}
p.shrink();
```

The ObjectPool can query it's memory usage.

```{cpp}
// Returns current sum total capacity (in bytes) of all Pool%s in the ObjectPool
std::cout << p.capacity() << std::endl;

// Returns current total capacity (in bytes) of the Pool that fits Data
std::cout << p.capacity<Data>() << std::endl;
```