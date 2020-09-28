// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

/*
Typesafe enum conversions.

// Example enum types:
enum class MyColor { my_Yellow, my_Green, my_Blue, my_Black };
enum class YourColor { your_White, your_Red, your_Blue, your_Yellow, your_Green, your_Blue };
// Old style enums (not strongly typed) are also still supported

// define these mappings once for each supported conversion:

CPP_BEGIN_ENUM_MAP(MyColor, YourColor)
CPP_ENUM_ENTRY(MyColor::my_Yellow, YourColor::your_Yellow)
CPP_ENUM_ENTRY(MyColor::my_Green, YourColor::your_Green)
CPP_ENUM_ENTRY(MyColor::my_Blue, YourColor::your_Blue)
CPP_END_ENUM_MAP()

CPP_BEGIN_ENUM_MAP(YourColor, MyColor)
CPP_ENUM_ENTRY(YourColor::your_Yellow, MyColor::my_Yellow)
CPP_ENUM_ENTRY(YourColor::your_Green, MyColor::my_Green)
CPP_ENUM_ENTRY(YourColor::your_Blue, MyColor::my_Blue)
CPP_END_ENUM_MAP()

CPP_BEGIN_ENUM_MAP(MyColor, std::string)
CPP_ENUM_ENTRY_STRING(MyColor::my_Yellow)
CPP_ENUM_ENTRY_STRING(MyColor::my_Green)
CPP_ENUM_ENTRY_STRING(MyColor::my_Blue)
CPP_ENUM_ENTRY_STRING(MyColor::my_Black)
CPP_ENUM_ENTRY_DEFAULT(x, "Unknown MyColor") // Optional override of default bad_enum_cast() exception
CPP_END_ENUM_MAP()

CPP_BEGIN_ENUM_MAP(MyColor, std::wstring)
CPP_ENUM_ENTRY_WSTRING(MyColor::my_Yellow)
CPP_ENUM_ENTRY_WSTRING(MyColor::my_Green)
CPP_ENUM_ENTRY_WSTRING(MyColor::my_Blue)
CPP_ENUM_ENTRY_WSTRING(MyColor::my_Black)
CPP_ENUM_ENTRY_DEFAULT(x, EnumToString(x)) // Optional override of default bad_enum_cast() exception
CPP_END_ENUM_MAP()

// Conversion examples:
enum_cast<YourColor>(MyColor::my_Green) // => YourColor::your_Green
enum_cast<MyColor>(YourColor::your_Yellow) // => MyColor::my_Yellow
enum_cast<MyColor>(YourColor::your_White) // => throws bad_enum_cast
enum_cast<std::string>(MyColor::my_Black) // => std::string("MyColor::my_Black")
enum_cast<std::wstring>(MyColor::my_Black) // => std::wstring(L"MyColor::my_Black")

*/

#include <typeinfo>
#include <cpplib/types/string_builder.h>

namespace cpp
{

class bad_enum_cast : public std::bad_cast
{
};

template <typename Src, typename Dst>
class enum_cast_exception : public bad_enum_cast
{
public:
    explicit enum_cast_exception(Src value) : m_value(value)
    {
    }

    virtual const char* what() const override
    {
        m_msg = stringbuilder() << "bad_enum_cast: enum_cast<" << typeid(Dst).name() << ">(" << typeid(Src).name() << "(" << static_cast<int>(m_value) << "))";
        return m_msg.c_str();
    }

private:
    Src m_value;
    mutable std::string m_msg;
};

} // namespace cpp

// Why en8092? The identifier should not clash with a typical argument for
// CPP_ENUM_ENTRY_DEFAULT(), that's why en or src are not appropriate here.
//-------------------------------------------------------------------

#define CPP_BEGIN_ENUM_MAP(from, to)  /*TICS !CFL#002*/             \
                                                                    \
template <typename Dst, typename Src>                               \
__declspec(deprecated("no such conversion")) Dst enum_cast(Src en); \
                                                                    \
template <>                                                         \
inline to enum_cast<to, from>(from en8092)                          \
{	                                                                \
	using to_type = to;                                             \
    using source_type = from;                                       \
	using exception_type = ::cpp::enum_cast_exception<from, to>;    \
	switch (en8092)                                                 \
	{
//-------------------------------------------------------------------
#define CPP_ENUM_ENTRY(from, to)                                    \
	case from: return (to);

//-------------------------------------------------------------------

#define CPP_ENUM_ENTRY_STRING(from)                                 \
	case from: return #from;

//-------------------------------------------------------------------

#define CPP_ENUM_ENTRY_WSTRING(from)                                \
	case from: return L ## #from;

//-------------------------------------------------------------------

#define CPP_END_ENUM_MAP_EX(from, to)                               \
	default:                                                        \
	    if (true)                                                   \
        {                                                           \
      		source_type from = en8092;                              \
		    static_cast<void>(from);                                \
		    return (to);                                            \
	    }                                                           \
    }	                                                            \
    throw exception_type(en8092);                                   \
}; 

//-------------------------------------------------------------------
#define CPP_END_ENUM_MAP()                                          \
    default:                                                        \
      {                                                             \
         break;                                                     \
      }                                                             \
	}	                                                            \
	throw exception_type(en8092);                                   \
};