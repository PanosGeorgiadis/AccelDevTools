# C++ Q&A

## Virtual destructors

*When should I use virtual destructors?*

Virtual destructors are useful when you might potentially delete an instance of a derived class through a pointer to a base class

```cpp
//header
class Base { /*declaration*/ };
class Derived : public Base { public: ~Derived() { } };
// end header
//
// implementation (cpp)
Base* b = new Derived();
// use b
delete b;
// end implementation
```

In the above example, if the Base destructor is not virtual, and b is Base* pointing to a Derived, `delete b` has undefined behavior

Answer: Always make base class destructors `virtual`. An abstract base class is an *interface*, typically used for implementing behavior on a derived class.


## Function pointers

### C++11 syntax

```cpp
typedef void *(MyFunctionPtr)(int x, int y, int z);
```

Using similar syntax:

```cpp
void calculateDistance(int a, int b, int c)
{
    cout << a*a + b*b + c*c;
}

using MyFunctionPtr = void (*) (int, int, int);

MyFunctionPtr fptr = calculateDistance;
fptr(1, 2, 3);

```

OR (with C++11 constructs):

```cpp
#include <type_traits>
using namespace std;

long calc(int x, int y)
{
    return (long)(x + y);
}
using MyFunctionPtr = std::add_pointer<long(int, int)>::type;
MyFunctionPtr fptr = calc;
cout << fptr(1, 2) << endl;
```

*What about `const unique_ptr`?*

- can be considered a better boost::scoped_ptr.
- can only transfer ownership away from it by using const_cast
- cannot be swapped

## move semantics: unique_ptr - brief tutorial by Howard E. Hinnant (2011-04-15)

*For developers previously using auto_ptr:*

`unique_ptr` is the C++11 replacement for `auto_ptr` which is now deprecated.

One can use this type in the exact same way as `auto_ptr`:

```cpp
unique_ptr<int> factory(int i)
{
    return unique_ptr<int>(new int(i));
}

void client(unique_ptr<int> p)
{
    // Ownership transferred into client
}   // int* deleted here

void test()
{
    unique_ptr<int> p = factory(2);
    // *p == 2
    p.reset(new int(3));
    // *p == 3
    client(factory(4));
}
```

Note in the example above that every time `unique_ptr` is copied, the source of the copy is an rvalue or temporary.

One can make a copy of an rvalue `unique_ptr`, but one can not make a copy of an lvalue `unique_ptr`. If you try, the compiler will issue a diagnostic:

```cpp
unique_ptr<int> factory(int i)
{
    return unique_ptr<int>(new int(i));
}

void client(unique_ptr<int> p)
{
    // Ownership transferred into client
}   // int* deleted here

void test()
{
    unique_ptr<int> p = factory(2);
    // *p == 2
    p.reset(new int(3));
    // *p == 3
    client(p);  // error: call to deleted constructor of unique_ptr<int>
}
```

In principle, `unique_ptr` is used to define *ownership*. While several `shared_ptr` objects may own the same object, a `unique_ptr` may not own an object also used by another `unique_ptr`.
It should not be confused with the singleton pattern which is used to assert there is only one instance of a type within a specific context.

```cpp
class Resources {/*...defined elsewhere*/}
class CpuProcess
{
    /*
    public interface
    */
private:
    unique_ptr<Resources> m_pResources;
};
```

