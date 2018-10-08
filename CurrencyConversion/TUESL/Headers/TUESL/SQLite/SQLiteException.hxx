#pragma once

// Use this to find out SQLite3 Error Message from Code

#include "SQLite3PCH.hxx"
#include <exception>

namespace TUESL::SQLite
{
	struct SQLiteException : public std::exception
	{
	private:
		int m_sql_error_code;

	public:

		explicit SQLiteException(const int p_sql_error_code) noexcept :
			std::exception{ sqlite3_errstr(p_sql_error_code) },
			m_sql_error_code{ p_sql_error_code }
		{}
		auto getErrorCode() const noexcept
		{
			return m_sql_error_code;
		}
	};
}
