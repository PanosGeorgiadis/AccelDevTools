# Function Pointers

## C++11 syntax

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

OR:

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
