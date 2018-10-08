#include "pch.h"
#include <TUESL/SQLite/PrepareStatement.hxx>

namespace TUESL::SQLite
{
	inline void PrepareStatement::verify(const int resultCode) const
	{
		if (resultCode != SQLITE_OK)
			throw SQLiteException(resultCode);
	}
	inline int
		 PrepareStatement::convertStringIndexToPosition(const std::string_view p_index)
	{
		return sqlite3_bind_parameter_index(m_stmt.get(), std::data(p_index));
	}
	inline void PrepareStatement::incrementCurrentGetIndex(const Index p_cur_index)
	{
		if (p_cur_index == m_get_cur_index)
			++m_get_cur_index;
		else
			m_get_cur_index = p_cur_index;
	}
	bool PrepareStatement::checkTableExistence(Database&					p_db,
															 const std::string_view p_tbl_name)
	{
		using namespace std::string_literals;

		// This Query asks the Sqlite_master table about the existence of given table
		const std::string sql =
			 "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name =?";

		prepare(p_db, sql);
		bind(p_tbl_name);

		return hasNext();
	}
	PrepareStatement& PrepareStatement::beginTransaction(Database& p_db)
	{
		return execute(p_db, "BEGIN IMMEDIATE TRANSACTION;");
	}
	PrepareStatement& PrepareStatement::endTransaction(Database& p_db)
	{
		return execute(p_db, "END TRANSACTION;");
	}

	PrepareStatement& PrepareStatement::prepare(Database&					 p_db,
															  const std::string_view p_sql)
	{
		if (m_stmt.hasValue())
			reset();

		const auto resultCode = sqlite3_prepare_v2(p_db.getDatabaseRAWHandle(),
																 std::data(p_sql),
																 static_cast<int>(std::size(p_sql)),
																 m_stmt.getAddressOf(),
																 nullptr);

		verify(resultCode);

		// Minimum Value of Index
		m_bind_cur_index = 1;
		m_get_cur_index  = 0;

		return *this;
	}

	PrepareStatement& PrepareStatement::prepare(Database&					  p_db,
															  const std::wstring_view p_sql)
	{
		// Verify if wchar_t is 16 Bit or Not
		// If it's not, then issue a static_assert
		static_assert(Utility::size_in_bytes<std::wstring_view::value_type> == 16,
						  "Error Occured. wchar_t must be a 16-bit type to use with SQLite");

		if (m_stmt.hasValue())
			reset();

		const auto resultCode = sqlite3_prepare16_v2(p_db.getDatabaseRAWHandle(),
																	std::data(p_sql),
																	static_cast<int>(std::size(p_sql)),
																	m_stmt.getAddressOf(),
																	nullptr);

		verify(resultCode);

		// Minimum Value of Index
		m_bind_cur_index = 1;
		m_get_cur_index  = 0;

		return *this;
	}

	inline std::string PrepareStatement::getPrepareSQLStatement() noexcept
	{
		return sqlite3_sql(m_stmt.get());
	}

	void PrepareStatement::reset()
	{
		// If It is Empty, what is it that has to be Reseted
		if (std::empty(m_stmt))
			return;

		const auto resetCode = sqlite3_reset(m_stmt.get());
		verify(resetCode);

		// sqlite3_reset does not clear Bindings!
		const auto clearBindingCode = sqlite3_clear_bindings(m_stmt.get());
		verify(clearBindingCode);

		// Release Statements so its Nulled and can be reused
		m_stmt.release();

		// Minimum Value of Index
		m_bind_cur_index = 1;
		m_get_cur_index  = 0;
	}

	bool PrepareStatement::isReadOnly() noexcept
	{
		if (std::empty(m_stmt))
			return false;
		return sqlite3_stmt_readonly(m_stmt.get());
	}

	std::optional<int> PrepareStatement::noOfColumns() noexcept
	{
		// If No Statements exist, this returns -1
		if (std::empty(m_stmt))
			return std::nullopt;

		return sqlite3_column_count(m_stmt.get());
	}

	bool PrepareStatement::hasNext()
	{
		if (std::empty(m_stmt))
			return false;

		const int resultCode = sqlite3_step(m_stmt.get());

		if (resultCode == SQLITE_ROW)
		{
			m_get_cur_index = 0;
			return true;
		}
		if (resultCode == SQLITE_DONE)
		{
			m_get_cur_index = 0;
			return false;
		}
		else
			throw SQLiteException(resultCode);
	}
	PrepareStatement& PrepareStatement::execute(Database&					 p_db,
															  const std::string_view p_sql)
	{
		return prepare(p_db, p_sql).execute();
	}
	PrepareStatement& PrepareStatement::execute(Database&					  p_db,
															  const std::wstring_view p_sql)
	{
		return prepare(p_db, p_sql).execute();
	}
	PrepareStatement& PrepareStatement::execute()
	{
		if (std::empty(m_stmt))
			return *this;

		const int resultCode = sqlite3_step(m_stmt.get());

		if (resultCode == SQLITE_ROW || resultCode == SQLITE_DONE)
			return *this; // As these Indicate Success
		else
			throw SQLiteException(resultCode);
	}

	std::optional<std::string>
		 PrepareStatement::getColumnName(const Index p_index) noexcept
	{
		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;

		return sqlite3_column_name(m_stmt.get(), static_cast<int>(p_index));
	}
	std::optional<double> PrepareStatement::getDouble(const Index p_index) noexcept
	{
		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;

		incrementCurrentGetIndex(p_index);

		return sqlite3_column_double(m_stmt.get(), static_cast<int>(p_index));
	}
	std::optional<int> PrepareStatement::getInteger(const Index p_index) noexcept
	{
		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;
		incrementCurrentGetIndex(p_index);

		return sqlite3_column_int(m_stmt.get(), static_cast<int>(p_index));
	}
	std::optional<DataTypes::Int64>
		 PrepareStatement::getInt64(const Index p_index) noexcept
	{
		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;
		incrementCurrentGetIndex(p_index);

		return sqlite3_column_int64(m_stmt.get(), static_cast<int>(p_index));
	}
	std::optional<std::string> PrepareStatement::getString(const Index p_index) noexcept
	{
		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;

		const auto value = sqlite3_column_text(m_stmt.get(), static_cast<int>(p_index));

		if (value == nullptr)
			return std::nullopt;

		incrementCurrentGetIndex(p_index);

		const std::string text = reinterpret_cast<const char*>(value);
		return text;
	}
	std::optional<std::wstring> PrepareStatement::getWString(const Index p_index) noexcept
	{
		// Verify if wchar_t is 16 Bit or Not
		// If it's not, then issue a static_assert
		static_assert(Utility::size_in_bytes<std::wstring_view::value_type> == 16,
						  "Error Occurred. wchar_t must be a 16-bit type to use with SQLite");

		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;

		const auto value = sqlite3_column_text16(m_stmt.get(), static_cast<int>(p_index));

		if (value == nullptr)
			return std::nullopt;

		incrementCurrentGetIndex(p_index);

		const std::wstring text = reinterpret_cast<const wchar_t*>(value);
		return text;
	}
#ifdef TUESL_USING_CPP_WINRT
	std::optional<winrt::hstring>
		 PrepareStatement::getHString(const Index p_index) noexcept
	{
		if (std::empty(m_stmt) || p_index < 0)
			return std::nullopt;

		const auto value = sqlite3_column_text16(m_stmt.get(), static_cast<int>(p_index));

		if (value == nullptr)
			return std::nullopt;

		incrementCurrentGetIndex(p_index);

		const winrt::hstring text = reinterpret_cast<const wchar_t*>(value);
		return text;
	}
#endif
	inline int PrepareStatement::getNoOfColumns() noexcept
	{
		return sqlite3_data_count(m_stmt.get());
	}

	// Provide Index to Bind in the form of a String
	PrepareStatement& PrepareStatement::bind(const std::string_view p_index,
														  const std::nullptr_t)
	{
		return bind(convertStringIndexToPosition(p_index), nullptr);
	}
	PrepareStatement& PrepareStatement::bind(const Index				 p_index,
														  const std::string_view p_value)
	{
		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (std::empty(m_stmt) || p_index < 1)
			return *this;

		const auto resultCode = sqlite3_bind_text(m_stmt.get(),
																static_cast<int>(p_index),
																std::data(p_value),
																static_cast<int>(std::size(p_value)),
																SQLITE_TRANSIENT);
		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
	PrepareStatement& PrepareStatement::bind(const Index				  p_index,
														  const std::wstring_view p_value)
	{
		// Verify if wchar_t is 16 Bit or Not
		// If it's not, then issue a static_assert
		static_assert(Utility::size_in_bytes<std::wstring_view::value_type> == 16,
						  "Error Occured. wchar_t must be a 16-bit type to use with SQLite");

		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (std::empty(m_stmt) || p_index < 1)
			return *this;

		const auto resultCode = sqlite3_bind_text16(m_stmt.get(),
																  static_cast<int>(p_index),
																  std::data(p_value),
																  static_cast<int>(std::size(p_value)),
																  SQLITE_TRANSIENT);
		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
#ifdef TUESL_USING_CPP_WINRT
	PrepareStatement& PrepareStatement::bind(const Index			  p_index,
														  const winrt::hstring p_value)
	{
		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (std::empty(m_stmt) || p_index < 1)
			return *this;

		const auto resultCode = sqlite3_bind_text16(m_stmt.get(),
																  static_cast<int>(p_index),
																  std::data(p_value),
																  -1,
																  SQLITE_TRANSIENT);

		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
#endif
	PrepareStatement& PrepareStatement::bind(const Index p_index, const double p_value)
	{
		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (m_stmt.empty() || p_index < 1)
			return *this;

		const auto resultCode =
			 sqlite3_bind_double(m_stmt.get(), static_cast<int>(p_index), p_value);
		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
	PrepareStatement& PrepareStatement::bind(const Index			p_index,
														  const std::int32_t p_value)
	{
		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (m_stmt.empty() || p_index < 1)
			return *this;

		const auto resultCode =
			 sqlite3_bind_int64(m_stmt.get(), static_cast<int>(p_index), p_value);
		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
	PrepareStatement& PrepareStatement::bind(const Index				 p_index,
														  const DataTypes::Int64 p_value)
	{
		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (m_stmt.empty() || p_index < 1)
			return *this;

		const auto resultCode =
			 sqlite3_bind_int64(m_stmt.get(), static_cast<int>(p_index), p_value);
		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
#ifdef TUESL_USING_CPP_WINRT
	PrepareStatement& PrepareStatement::bind(const Index p_index, const TimeSpan& p_value)
	{
		// Count Returns the Time Elapsed in Units
		// Since the Epoch
		return bind(p_index, p_value.count());
	}
#endif
	PrepareStatement& PrepareStatement::bind(const Index p_index, const std::nullptr_t)
	{
		// If It is Empty, what is it that has to be binded
		// Minimum Value of Index is 1
		if (m_stmt.empty() || p_index < 1)
			return *this;

		const auto resultCode = sqlite3_bind_null(m_stmt.get(), static_cast<int>(p_index));
		verify(resultCode);

		++m_bind_cur_index;

		return *this;
	}
} // namespace TUESL::SQLite