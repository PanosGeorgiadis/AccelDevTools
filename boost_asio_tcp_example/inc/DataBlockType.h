#pragma once

#include <inc/enum_cast.h>
#include "CeosProtocol/Constants.h"

namespace Server {
namespace CeosProtocol {

struct DataBlockType
{
    enum type
    {
        Unknown,
        Char,
        Float,
        Double,
        Integer,
        UnsignedInteger,
        Short,
        UnsignedShort,
        Bool,
        RawData
    };
};

BEGIN_ENUM_MAP(DataBlockType::type, std::wstring)
    T_ENUM_ENTRY(DataBlockType::Unknown, L"Unknown")
    T_ENUM_ENTRY(DataBlockType::Char, L"Char")
    T_ENUM_ENTRY(DataBlockType::Float, L"Float")
    T_ENUM_ENTRY(DataBlockType::Double, L"Double")
    T_ENUM_ENTRY(DataBlockType::Integer, L"Integer")
    T_ENUM_ENTRY(DataBlockType::UnsignedInteger, L"Unsigned Integer")
    T_ENUM_ENTRY(DataBlockType::Short, L"Short")
    T_ENUM_ENTRY(DataBlockType::UnsignedShort, L"Unsigned Short")
    T_ENUM_ENTRY(DataBlockType::Bool, L"Bool")
    T_ENUM_ENTRY(DataBlockType::RawData, L"Raw Data")
END_ENUM_MAP()

BEGIN_ENUM_MAP(DataBlockType::type, unsigned int)
    T_ENUM_ENTRY(DataBlockType::Unknown, CeosProtocol::DATA_TYPE_UNKNOWN)
    T_ENUM_ENTRY(DataBlockType::Char, CeosProtocol::DATA_TYPE_CHAR)
    T_ENUM_ENTRY(DataBlockType::Float, CeosProtocol::DATA_TYPE_FLOAT)
    T_ENUM_ENTRY(DataBlockType::Double, CeosProtocol::DATA_TYPE_DOUBLE)
    T_ENUM_ENTRY(DataBlockType::Integer, CeosProtocol::DATA_TYPE_INTEGER)
    T_ENUM_ENTRY(DataBlockType::UnsignedInteger, CeosProtocol::DATA_TYPE_UNSIGNED_INTEGER)
    T_ENUM_ENTRY(DataBlockType::Short, CeosProtocol::DATA_TYPE_SHORT)
    T_ENUM_ENTRY(DataBlockType::UnsignedShort, CeosProtocol::DATA_TYPE_UNSIGNED_SHORT)
    T_ENUM_ENTRY(DataBlockType::Bool, CeosProtocol::DATA_TYPE_BOOL)
    T_ENUM_ENTRY(DataBlockType::RawData, CeosProtocol::DATA_TYPE_RAW_DATA)
END_ENUM_MAP()

BEGIN_ENUM_MAP(unsigned int, DataBlockType::type)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_UNKNOWN, DataBlockType::Unknown)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_CHAR, DataBlockType::Char)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_FLOAT, DataBlockType::Float)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_DOUBLE, DataBlockType::Double)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_INTEGER, DataBlockType::Integer)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_UNSIGNED_INTEGER, DataBlockType::UnsignedInteger)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_SHORT, DataBlockType::Short)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_UNSIGNED_SHORT, DataBlockType::UnsignedShort)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_BOOL, DataBlockType::Bool)
    T_ENUM_ENTRY(CeosProtocol::DATA_TYPE_RAW_DATA, DataBlockType::RawData)
END_ENUM_MAP()

} // namespace Ceos
} // namespace Server
