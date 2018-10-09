#pragma once

#include "SQLHandler.hxx"
#include "SQLiteException.hxx"

namespace TUESL::SQLite
{
	namespace Version
	{
		// Refer to this for URL
		// For ways to obtain URL
		// https://www.sqlite.org/c3ref/c_source_id.html

		// This is the RAW Version Number
		// Probably Not Preferable in most cases
		constexpr const auto LIB_VERSION_RAW = SQLITE_VERSION_NUMBER;

		// The Major Version
		constexpr const auto MAJOR = LIB_VERSION_RAW / 1000000;
		// The Minor Version
		constexpr const auto MINOR = (LIB_VERSION_RAW / 1000) % (MAJOR * 1000);
		// The PATCH
		constexpr const auto PATCH = ((LIB_VERSION_RAW) % 1000000) % (1000);
	}

	class Database
	{
	 private:
		Handler::Database m_db;

	 public:
		Database(const std::string_view p_file_name,
					const int p_flags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE |
											  SQLITE_OPEN_FULLMUTEX)
		{
			open(p_file_name, p_flags);
		}
		Database& open(const std::string_view p_file_name,
							const int p_flags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE |
													  SQLITE_OPEN_FULLMUTEX);

		static bool IsThreadingEnabled() noexcept
		{
			// Returns 0 if Threading Disabled
			// 1 Otherwise
			return sqlite3_threadsafe() != 0;
		}

		Database& transactionBegin();
		Database& transactionRollback();
		Database& transactionEnd();

		bool isReadOnly() const noexcept;

		Database& executeSQL(const std::string_view p_sql);

		int  errorCode() const noexcept;
		int  errorExtendedCode() const noexcept;
		auto errorMessageString() const noexcept;

		inline Handler::Database::POINTER getDatabaseRAWHandle() noexcept
		{
			return m_db.get();
		}
	};
} // namespace TUESL::SQLite
