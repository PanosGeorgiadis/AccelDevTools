// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <sstream>
#include <iomanip>
#include <cmath>

//TICS -OLC#009 "Avoid the use of so-called "magic numbers" in arbitrary expressions"

/*
* expformat.h - exponential formatting of floating point values
*
* scientific and engineering exponential formats are available as m x 10^e
* where for scientific format 1 <= m < 10
* and for engineering format 1 <= m < 1000, e = 3n
*
* scientific(double v, digits dig)
* scientific(double v, digits dig, const char* unit)
* scientific(double v, digits dig, const wchar_t* unit)
* scientific(double v, decimals dec)
* scientific(double v, decimals dec, const char* unit)
* scientific(double v, decimals dec, const wchar_t* unit)
*
* engineering(double v, digits dig)
* engineering(double v, digits dig, const char* unit)
* engineering(double v, digits dig, prefixed, const char* unit)
* engineering(double v, digits dig, const wchar_t* unit)
* engineering(double v, digits dig, prefixed, const wchar_t* unit)
* engineering(double v, decimals dec)
* engineering(double v, decimals dec, const char* unit)
* engineering(double v, decimals dec, prefixed, const char* unit)
* engineering(double v, decimals dec, const wchar_t* unit)
* engineering(double v, decimals dec, prefixed, const wchar_t* unit)
*
* Arguments:
*
* v: the floating point value to format
*
* dig: the number of digits to print
*
* dec: the number of digits to print after the decimal point
*
* unit: the unit text to attach to the formatted value
*
* prefixed: Add this to add an exponential prefix to the unit rather than use plain exponential notation.
*
* These iostream fmtflags are supported:
*     showpos:   Add explicit + sign to positive values
*     showpoint: Insert a decimal point unconditionally in generated mantisse field
*     uppercase: Use E rather than e as exponent prefix
*     width():   The width setting applies to the output
*
* Examples:
*
* #include "Imaging/expformat.h"
*
* double v = 123456.789;
* cout << scientific(v, decimals(2));               // "1.23e5"
* cout << scientific(v, digits(4), "V");            // "1.235e5 V"
* cout << engineering(v, decimals(2));              // "123.46e3"
* cout << engineering(v, digits(2));                // "0.12e6"
* cout << engineering(v, decimals(2), "V");         // "123.46e3 V"
* cout << engineering(v, digits(4), prefixed, "V"); // "123.5 kV"
*/

#pragma once

#include <sstream>
#include <iomanip>
#include <cmath>

#pragma once

namespace cpp
{
namespace format
{

struct decimals
{
    explicit decimals(int v) :
        value(v)
    {
    }

    int value;
};

struct digits
{
    explicit digits(int v) :
        value(v)
    {
    }

    int value;
};

class exponential_formatter
{
public:
    exponential_formatter(double val, decimals d, int base, bool prefix) :
        prefix_(0)
    {
        init_decimals(val, d.value, base);
        init_prefix(prefix);
    }

    exponential_formatter(double val, digits d, int base, bool prefix) :
        prefix_(0)
    {
        init_digits(val, d.value, base);
        init_prefix(prefix);
    }

    bool prefixed() const
    {
        return prefix_ != 0;
    }

    template <typename CharT>
    friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const exponential_formatter& e)
    {
        if (_isnan(e.val_))
            return os << "NaN";

        std::ios_base::fmtflags fmt = os.flags();
        std::basic_ostringstream<CharT> ss;
        ss.flags(fmt & std::ios_base::showpoint);
        if ((fmt & std::ios_base::showpos) && e.val_ >= 0.)
        {
            ss << os.widen('+');
        }

        if (!_finite(e.val_))
        {
            if (e.val_ < 0)
            {
                ss << os.widen('-');
            }
            ss << "Inf";
            return os << ss.str();
        }

        ss << std::fixed << std::setprecision(e.decimals_) << e.val_*pow(10., -e.exp_);
        if (e.prefix_)
        {
            ss << os.widen(' ') << os.widen(e.prefix_);
        }
        else if (e.exp_)
        {
            ss << os.widen((fmt & std::ios_base::uppercase) ? 'E' : 'e') << e.exp_;
        }

        return os << ss.str();
    }

private:
    int getexp(double val, int decimals, int base) const
    {
        if (val == 0)
        {
            return 0;
        }

        // This calculation gets the exponential base, then rounds to the precision.
        // Rounding might change the exponential base, so it recalculates the exp base after rounding
        // Then it normalizes val based on exp as a base 10 exponent.
        int exp = base * int(floor(log10(fabs(val)) / base));
        val = floor(val*pow(10., decimals - exp) + 0.5);
        return base * static_cast<int>(floor(log10(fabs(pow(10., exp - decimals)*val)) / base));
    }

    void init_decimals(double val, int decimals, int base)
    {
        val_ = val;
        exp_ = getexp(val, decimals, base);
        decimals_ = decimals;
    }

    void init_digits(double val, int digits, int base)
    {
        int decimals = digits - 1;
        int exp = getexp(val, decimals, 1);

        int rem = exp % base;
        if (rem < 0)
        {
            rem += base;
        }

        // If we have more digits than requested, shift them after the decimal.
        if (rem > decimals)
        {
            decimals += base;
            exp += base;
        }
        val_ = val;
        exp_ = exp - rem;
        decimals_ = decimals - rem;
    }

    void init_prefix(bool prefix)
    {
        if (prefix && exp_ % 3 == 0)
        {
            if (exp_ >= -24 && exp_ < 0)
            {
                prefix_ = "mµnpfazy"[-exp_ / 3 - 1];
            }
            else
            {
                if (exp_ > 0 && exp_ <= 24)
                {
                    prefix_ = "kMGTPEZY"[exp_ / 3 - 1];
                }
            }
        }
    }

    double val_;
    int exp_;
    std::streamsize decimals_;
    char prefix_;
};

template <typename CharT>
class unit_formatter
{
public:
    unit_formatter(double val, decimals d, int base, bool prefix, const CharT* unit) :
        fmt_(val, d, base, prefix), unit_(unit)
    {
    }

    unit_formatter(double val, digits d, int base, bool prefix, const CharT* unit) :
        fmt_(val, d, base, prefix), unit_(unit)
    {
    }

    friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const unit_formatter<CharT>& e)
    {
        std::basic_ostringstream<CharT> ss;
        ss.flags(os.flags() & (std::ios_base::showpos | std::ios_base::uppercase));
        ss << e.fmt_;
        if (!e.fmt_.prefixed())
        {
            ss << os.widen(' ');
        }
        ss << e.unit_;
        return os << ss.str();
    }

private:
    exponential_formatter fmt_;
    const CharT* unit_;
};

enum prefixed_t { prefixed };

inline exponential_formatter scientific(double v, decimals d)
{
    return exponential_formatter(v, d, 1, false);
}

template <typename CharT>
inline unit_formatter<CharT> scientific(double v, decimals d, const CharT* unit)
{
    return unit_formatter<CharT>(v, d, 1, false, unit);
}

inline exponential_formatter scientific(double v, digits d)
{
    return exponential_formatter(v, d, 1, false);
}

template <typename CharT>
inline unit_formatter<CharT> scientific(double v, digits d, const CharT* unit)
{
    return unit_formatter<CharT>(v, d, 1, false, unit);
}

inline exponential_formatter engineering(double v, decimals d)
{
    return exponential_formatter(v, d, 3, false);
}

template <typename CharT>
inline unit_formatter<CharT> engineering(double v, decimals d, const CharT* unit)
{
    return unit_formatter<CharT>(v, d, 3, false, unit);
}

template <typename CharT>
inline unit_formatter<CharT> engineering(double v, decimals d, prefixed_t, const CharT* unit)
{
    return unit_formatter<CharT>(v, d, 3, true, unit);
}

inline exponential_formatter engineering(double v, digits d)
{
    return exponential_formatter(v, d, 3, false);
}

template <typename CharT>
inline unit_formatter<CharT> engineering(double v, digits d, const CharT* unit)
{
    return unit_formatter<CharT>(v, d, 3, false, unit);
}

template <typename CharT>
inline unit_formatter<CharT> engineering(double v, digits d, prefixed_t, const CharT* unit)
{
    return unit_formatter<CharT>(v, d, 3, true, unit);
}

} // format
} // cpp
