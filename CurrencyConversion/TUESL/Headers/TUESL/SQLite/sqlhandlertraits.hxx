#pragma once

#include "../Utility/UniqueHandler.hxx"
#include "SQLite3PCH.hxx"

namespace TUESL::SQLite::Traits
{
	struct SQLiteDatabaseHandlerTraits
	{
		using POINTER		  = sqlite3*;
		using CONST_POINTER = const POINTER;

		static auto invalid() noexcept
		{
			return nullptr;
		}
		static auto close(POINTER p_connection)
		{
			VERIFY_FUNCTION(SQLITE_OK, sqlite3_close_v2(p_connection));
		}
	};
	struct SQLitePrepareStatementHandlerTraits
	{
		using POINTER		  = sqlite3_stmt*;
		using CONST_POINTER = const POINTER;

		static auto invalid() noexcept
		{
			return nullptr;
		}
		static auto close(POINTER p_connection)
		{
			VERIFY_FUNCTION(SQLITE_OK, sqlite3_finalize(p_connection));
		}
	};
} // namespace TUESL::SQLite::Traits
