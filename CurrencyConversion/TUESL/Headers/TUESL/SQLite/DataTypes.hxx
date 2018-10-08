#pragma once

#include "SQLite3PCH.hxx"

namespace TUESL::SQLite::DataTypes
{
	using Int64 = sqlite3_int64;
	using UInt64 = sqlite3_uint64;
}

namespace TUESL::SQLite::Type
{
	enum class OrderBy
	{
		ASC, 		//Ascending Order
		DESC		//Descending Order
	};
}

