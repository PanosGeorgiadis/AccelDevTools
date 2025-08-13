// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{

enum class SIUnits
{
    s,
    m,
    Kg,
    A,
    K,
    mol,
    cd
};

enum class SIDimension
{
    Time,
    Length,
    Mass,
    Current,
    Temperature,
    Substance,
    Luminosity
};


class Meter final
{
public:
    explicit Meter(double value) :
        m_value{ value }
    {
    }

    Meter operator*=(double factor)
    {
        return Meter(factor * m_value);
    }

    double value() const
    {
        return m_value;
    }

    bool operator==(const Meter& other)
    {
        return (m_value == other.m_value);
    }

private:
    double m_value;
};

// SI
Meter operator "" _ym(long double value) { return Meter(static_cast<double>(1.0e-24 * value)); }
Meter operator "" _zm(long double value) { return Meter(static_cast<double>(1.0e-21 * value)); }
Meter operator "" _am(long double value) { return Meter(static_cast<double>(1.0e-18 * value)); }
Meter operator "" _fm(long double value) { return Meter(static_cast<double>(1.0e-15 * value)); }
Meter operator "" _pm(long double value) { return Meter(static_cast<double>(1.0e-12 * value)); }
Meter operator "" _nm(long double value) { return Meter(static_cast<double>(1.0e-09 * value)); }
Meter operator "" _μm(long double value) { return Meter(static_cast<double>(1.0e-06 * value)); }
Meter operator "" _mm(long double value) { return Meter(static_cast<double>(1.0e-03 * value)); }
Meter operator ""  _m(long double value) { return Meter(static_cast<double>(1.0e+00 * value)); }
Meter operator "" _km(long double value) { return Meter(static_cast<double>(1.0e+03 * value)); }
Meter operator "" _Mm(long double value) { return Meter(static_cast<double>(1.0e+06 * value)); }
Meter operator "" _Gm(long double value) { return Meter(static_cast<double>(1.0e+09 * value)); }
Meter operator "" _Tm(long double value) { return Meter(static_cast<double>(1.0e+12 * value)); }
Meter operator "" _Pm(long double value) { return Meter(static_cast<double>(1.0e+15 * value)); }
Meter operator "" _Em(long double value) { return Meter(static_cast<double>(1.0e+18 * value)); }
Meter operator "" _Zm(long double value) { return Meter(static_cast<double>(1.0e+21 * value)); }
Meter operator "" _Ym(long double value) { return Meter(static_cast<double>(1.0e+24 * value)); }

// Non standard
Meter operator "" _cm(long double value) { return Meter(static_cast<double>(1.0e-02 * value)); }
Meter operator "" _Å(long double value) { return Meter(static_cast<double>(1.0e-10 * value)); }

// Imperial
//Meter operator "" _y

} // cpp
