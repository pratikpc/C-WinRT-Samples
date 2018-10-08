#include "pch.h"
#include <TUESL/SQLite/Database.hxx>

namespace TUESL::SQLite
{
	Database& Database::open(const std::string_view p_file_name, const int p_flags)
	{
		if (std::empty(p_file_name))
			return *this;

		// Note that creation of local ensures that
		// In case of any error in creation of database
		// The main database value, m_db will not get corrupted
		Handler::Database local{};

		const auto			result =
			 sqlite3_open_v2(std::data(p_file_name), local.getAddressOf(), p_flags, nullptr);

		if (result != SQLITE_OK)
			throw SQLiteException(result);

		m_db = std::move(local);

		return *this;
	}
	bool Database::isReadOnly() const noexcept
	{
		if (std::empty(m_db))
			return false;
		return sqlite3_db_readonly(m_db.get(), nullptr);
	}
	 Database& Database::executeSQL(const std::string_view p_sql)
	{
		if (m_db.hasValue() && !std::empty(p_sql))
		{
			const auto result =
				 sqlite3_exec(m_db.get(), std::data(p_sql), nullptr, nullptr, nullptr);
			if (result != SQLITE_OK)
				throw SQLiteException(result);
		}
		return *this;
	}
	inline int Database::errorCode() const noexcept
	{
		return sqlite3_errcode(m_db.get());
	}
	inline int Database::errorExtendedCode() const noexcept
	{
		return sqlite3_extended_errcode(m_db.get());
	}
	inline auto Database::errorMessageString() const noexcept
	{
		return sqlite3_errmsg(m_db.get());
	}
} // namespace TUESL::SQLite
