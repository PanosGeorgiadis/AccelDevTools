// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <string>
#include <cpplib/types/uuid.h>
#include "NonCopyable.h"

namespace cpp
{

class UniqueObject /*:
	public NonCopyable*/
{
public:
	UniqueObject() : m_uuid(uuid::new_uuid()) {}
	virtual ~UniqueObject() = default;

	// copies will have same uuid and will be considered the same object
	UniqueObject(const UniqueObject&) = default;
	UniqueObject& operator=(const UniqueObject&) = default;

	operator const uuid&() const noexcept
	{
		return m_uuid;
	}

	const uuid& get_id() const noexcept
	{
		return m_uuid;
	}

	const bool operator==(const UniqueObject& other)
	{
		return (m_uuid == other.m_uuid);
	}

	const bool operator!=(const UniqueObject& other)
	{
		return !(*this == other);
	}

private:
	const uuid m_uuid;
};

}