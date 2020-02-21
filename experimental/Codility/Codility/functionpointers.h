#pragma once
#include <type_traits>

long calc(int x, int y)
{
	return (long)(x + y);
}

void run_functionpointers()
{
	using MyFunctionPtr = std::add_pointer<long(int, int)>::type;
	MyFunctionPtr fptr = calc;
	cout << fptr(1, 2) << endl;
}
