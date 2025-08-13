
# C++ fast-track

## S.O.L.I.D. principles

_are guidelines, not rules_

* Single responsibility principle
* Open-Closed principle
* Liskov substitution
* Interface segregation
* Dependency inversion

## Stack vs Heap

**STACK**

- data for a single thread
- local vars & bookkeeping
- LIFO
- fixed-size (if not enough => stack overflow)
- faster than HEAP
- auto-dealloc when out of scope

**HEAP**

- dynamic data (i.e. raw pointers: a shared_ptr is stored in the stack, the thing it points to is in the heap)
- need to manually dealloc/delete (unless there is garbage collection)
- fragmentation possible
- one per application
- can grow as needed


## Clean Architecture

- separation of concerns
- consistent coding style, naming conventions
- modularity (reusability, independency, clear responsibility)
- small & focused functions
- comments/documentation
- no code duplication (more general: apply code metrics)
- testability
- version control

## OOP key components

- composition
- encapsulation
- inheritance
- polymorphism

## static vs dynamic polymorphism

**static**

- inlining
- compile-time
- less run-timme overhead
- early error detection
- example: method overloading
- static binding
- same class
- Curiously recurring template pattern (CRTP)

__example__

```cpp
// duckTyping.cpp

#include <chrono>
#include <iostream>

auto start = std::chrono::steady_clock::now();

void writeElapsedTime(){
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = now - start;
  
    std::cerr << diff.count() << " sec. elapsed: ";
}

struct MessageSeverity{
  void writeMessage() const {
      std::cerr << "unexpected" << '\n';
  }
};

struct MessageInformation {
  void writeMessage() const {              
    std::cerr << "information" << '\n';
  }
};

struct MessageWarning {
  void writeMessage() const {               
    std::cerr << "warning" << '\n';
  }
};

struct MessageFatal: MessageSeverity{};     

template <typename T>
void writeMessage(T& messServer){    // (1)                   
	
	writeElapsedTime();                                   
	messServer.writeMessage();                            
	
}

int main(){
	// writeMessage() will fail at compile-time if the argument doesn't support writeMessage()
    std::cout << '\n';
  
    MessageInformation messInfo;
    writeMessage(messInfo);
    
    MessageWarning messWarn;
    writeMessage(messWarn);

    MessageFatal messFatal;
    writeMessage(messFatal);
  
    std::cout << '\n';

}

```

**dynamic**

- run-time: performed using virtual functions



## GoF Design Patterns

**Creational**

create objects, rather than having to instantiate objects directly. This gives the program more flexibility in deciding which objects need to be created for a given case.

- Abstract factory groups object factories that have a common theme.
- Builder constructs complex objects by separating construction and representation.
- Factory method creates objects without specifying the exact class to create.
- Prototype creates objects by cloning an existing object.
- Singleton restricts object creation for a class to only one instance.

**Structural**

concern class and object composition. They use inheritance to compose interfaces and define ways to compose objects to obtain new functionality.

- Adapter allows classes with incompatible interfaces to work together by wrapping its own interface around that of an already existing class.
- Bridge decouples an abstraction from its implementation so that the two can vary independently.
- Composite composes zero-or-more similar objects so that they can be manipulated as one object.
- Decorator dynamically adds/overrides behavior in an existing method of an object.
- Facade provides a simplified interface to a large body of code.
- Flyweight reduces the cost of creating and manipulating a large number of similar objects.
- Proxy provides a placeholder for another object to control access, reduce cost, and reduce complexity.

**Behavioral**

concerned with communication between objects

- Chain of responsibility delegates commands to a chain of processing objects.
- Command creates objects that encapsulate actions and parameters.
- Interpreter implements a specialized language.
- Iterator accesses the elements of an object sequentially without exposing its underlying representation.
- Mediator allows loose coupling between classes by being the only class that has detailed knowledge of their methods.
- Memento provides the ability to restore an object to its previous state (undo).
- Observer is a publish/subscribe pattern, which allows a number of observer objects to see an event.
- State allows an object to alter its behavior when its internal state changes.
- Strategy allows one of a family of algorithms to be selected on-the-fly at runtime.
Template method defines the skeleton of an algorithm as an abstract class, allowing its subclasses to provide concrete behavior.
- Visitor separates an algorithm from an object structure by moving the hierarchy of methods into one object.



## 3 types of errors

- syntax
- runtime
- logical

## Rule of three/five/zero

## Dependency injection

## GRASP patterns/principles

- information expert
	related: information hiding
- creator
	related: factory pattern, low coupling
- controller
	related: command, facade, layers, pure fabrication
- indirection
- low coupling
	related: loose coupling, service-oriented architecture, dynamic binding
- high cohesion
- polymorphism
- protected variations
	related: open/closed principle, interfacing
- pure fabrication
	related: low coupling, high cohesion


