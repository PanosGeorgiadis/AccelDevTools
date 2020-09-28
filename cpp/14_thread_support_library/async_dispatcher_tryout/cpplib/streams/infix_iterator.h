// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <ostream>
#include <iterator>

namespace cpp
{
namespace streams
{

template <typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
class infix_ostream_iterator 
{
public:
    using iterator_category = std::output_iterator_tag;
    using value_type = CharT;
    using difference_type = CharT;
    using pointer = CharT * ;
    using reference = CharT & ;

    using char_type = CharT;
    using traits_type = Traits;
    using ostream_type = std::basic_ostream<CharT, Traits>;

    infix_ostream_iterator(ostream_type& s) :
        os(&s), delimiter(0), first_elem(true)
    {
    }

    infix_ostream_iterator(ostream_type& s, const CharT* d) :
        os(&s), delimiter(d), first_elem(true)
    {
    }

    infix_ostream_iterator& operator=(const T& item)
    {
        // Here's the only real change from ostream_iterator:
        // Normally, the '*os << item;' would come before the 'if'.
        if (!first_elem && delimiter != 0)
        {
            *os << delimiter;
        }

        *os << item;
        first_elem = false;
        return *this;
    }

    infix_ostream_iterator& operator*()
    {
        return *this;
    }

    infix_ostream_iterator& operator++()
    {
        return *this;
    }

    infix_ostream_iterator& operator++(int)
    {
        return *this;
    }

private:
    std::basic_ostream<CharT, Traits>* os;
    const CharT* delimiter;
    bool first_elem;
};

} // streams
} // cpp

namespace std 
{

template <typename T, typename CharT, typename Traits>
struct _Is_checked_helper<cpp::streams::infix_ostream_iterator<T, CharT, Traits>> : public std::true_type
{
};

} // namespace std


