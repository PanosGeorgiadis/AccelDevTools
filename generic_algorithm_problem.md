## Problem

Given a project P let projects D1, D2, D3 be its build dependencies 
the definition of the project's build order can then be written as:
P D1 D2 D3
A solution consists of multiple projects with their respective dependencies. To build the whole solution, all projects must be built in such an order that dependencies are built before the project

Given a list of project dependencies, order the projects so that the solution is built correctly

### Rules

Each project has at most one line of text defining its build dependencies
    The first item in the line is the project itself
    Any consecutive items are the project's dependencies
If a project has no dependencies it may not appear as a first item in any line

** Another example **

Let solution contain projects A B C D E with the following dependencies:

(input)
B A D
C A
D A E

Then a build order that satisfies these dependencies will be

(output)
E A D C B
