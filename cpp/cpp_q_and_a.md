# C++ Q&A

1. When should I use virtual destructors?

Virtual destructors are useful when you might potentially delete an instance of a derived class through a pointer to a base class

    //header
    class Base {};
    class Derived : public Base { public: ~Derived() { } };
    // end header
    //
    // implementation (cpp)
    Base* b = new Derived();
    // use b
    delete b;
    // end implementation

=> If the Base destructor is not virtual, and b is Base* pointing to a Derived, `delete b` has undefined behavior

Answer: Always make base class destructors `virtual`


