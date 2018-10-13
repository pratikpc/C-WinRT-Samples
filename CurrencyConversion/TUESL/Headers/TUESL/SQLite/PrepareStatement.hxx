#pragma once

#include <iostream>
#include <optional>
#include <string>

#if __has_include("winrt/Windows.Foundation.h")
// This definition is defined only when C++WinRT is being used
#	ifndef TUESL_USING_CPP_WINRT
#		define TUESL_USING_CPP_WINRT
#	endif
#else
#	ifdef TUESL_USING_CPP_WINRT
#		undef TUESL_USING_CPP_WINRT
#	endif
#endif

#ifdef TUESL_USING_CPP_WINRT
#	include <winrt/Windows.Foundation.h>
#endif

#ifdef TUESL_USING_CPP_WINRT
namespace TUESL::SQLite
{
	namespace
	{
		using winrt::hstring;

		using winrt::Windows::Foundation::DateTime;
		using winrt::Windows::Foundation::TimeSpan;
	} // namespace
} // namespace TUESL::SQLite
#endif

#include <TUESL/Utility/Utility.hxx>

#include <TUESL/SQLite/DataTypes.hxx>
#include <TUESL/SQLite/Database.hxx>
#include <TUESL/SQLite/SQLHandler.hxx>

namespace TUESL::SQLite
{

	// This is a Very Simple way on how to create a PrepareStatement
	// Use this for Queries which primarily return data
	// This allows Java-esque Iteration over Data
	// Example
	//	while(ps.hasNext())
	//		ps.get<int>();

	// However so as to Keep Loops Clean and short,
	// Use Function getDetails(Function func) and Provide a Lambda
	// This Lambda is in the form
	//			details(/*IMP*/ PrepareStatement& ps)
	// Via this, rather than performing a conversion in the User's own Loop,
	// a Previously created Lambda can be used
	// The Value returned by details Lambda provides the neccesary data!

	// It's presence shall shorten the code to
	//
	// while(ps.hasNext())
	//		employees.add(ps.getDetails(lambda_extract_details));

	// Note that the get methods return Optional rather than throw exceptions
	// While exception handling is used, it is not used purely!

	struct PrepareStatement
	{
	 private:
		using Index = std::size_t;

		Handler::PrepareStatement m_stmt{};

		// Stores the Counter for the Values to be Binded
		// In case the User does not want to provide
		// Integer or String based formatting for Index
		// Minimum Value of Index for Binding is 1
		Index m_bind_cur_index = 1;

		// Stores the Counter for the values to be Received
		// Minimum Current Index that can be received is 0
		Index m_get_cur_index = 0;

	 private:
		void verify(const int resultCode) const;

		int convertStringIndexToPosition(const std::string_view p_index) noexcept;

		void incrementCurrentBindIndex(const Index p_bind_cur_index) noexcept;
		void incrementCurrentGetIndex(const Index p_get_cur_index) noexcept;

	 public:
		PrepareStatement() {}
		PrepareStatement(Database& p_db, const std::string_view p_sql)
		{
			prepare(p_db, p_sql);
		}

		bool checkTableExistence(Database& p_db, const std::string_view p_tbl_name);

		PrepareStatement& prepare(Database& p_db, const std::string_view p_sql);
		PrepareStatement& prepare(Database& p_db, const std::wstring_view p_sql);

		std::string getPrepareSQLStatement() noexcept;

		void reset();

		bool isReadOnly() noexcept;

		std::optional<int> noOfColumns() noexcept;

		// Useful for Iterating over a Loop
		// Use it in the form
		// while(ps.hasNext())
		//	Use ps.get<Type>() to obtain values

		bool hasNext();

		PrepareStatement& execute(Database& p_db, const std::string_view p_sql);
		PrepareStatement& execute(Database& p_db, const std::wstring_view p_sql);

		PrepareStatement& execute();

		// However so as to Keep Loops Clean and short,
		// Use Function getDetails(Function func) and Provide a Lambda
		// This Lambda is in the form
		// details(/*IMP*/ PrepareStatement& ps)
		// Via this, rather than performing a conversion in the User's own Loop,
		// a Previously created Lambda can be used
		// The Value returned by details Lambda provides the neccesary data!

		// It's presence shall shorten the code to
		// while(ps.hasNext())
		//		employees.add(ps.getDetails(lambda_extract_details));

		template <typename Function>
		auto getDetails(Function func)
		{
			// static_assert(std::is_function_v(func), "The Provided Interface to extract
			// details must either be a Function or a Lambda");
			return func(*this);
		}

		std::optional<std::string> getColumnName(const Index p_index) noexcept;

		std::optional<double>			  getDouble(const Index p_index) noexcept;
		std::optional<int>				  getInteger(const Index p_index) noexcept;
		std::optional<DataTypes::Int64> getInt64(const Index p_index) noexcept;

		std::optional<std::string>  getString(const Index p_index) noexcept;
		std::optional<std::wstring> getWString(const Index p_index) noexcept;
#ifdef TUESL_USING_CPP_WINRT
		std::optional<winrt::hstring> getHString(const Index p_index) noexcept;
#endif

		template <typename ColumnType>
		auto get(const Index p_index) noexcept
		{
			using ColumnCheck = std::remove_cv_t<std::remove_reference_t<ColumnType>>;

			if constexpr (std::is_same_v<ColumnCheck, double>)
				return getDouble(p_index);
			else if constexpr (std::is_same_v<ColumnCheck, DataTypes::Int64>)
				return getInt64(p_index);
			else if constexpr (std::is_integral_v<ColumnCheck>)
				return getInteger(p_index);
			else if constexpr (std::is_same_v<ColumnCheck, std::string> ||
									 std::is_same_v<ColumnCheck, char*> ||
									 std::is_same_v<ColumnCheck, std::string_view>)
				return getString(p_index);
			else if constexpr (std::is_same_v<ColumnCheck, std::wstring> ||
									 std::is_same_v<ColumnCheck, wchar_t*> ||
									 std::is_same_v<ColumnCheck, std::wstring_view>)
				return getWString(p_index);
#ifdef TUESL_USING_CPP_WINRT
			else if constexpr (std::is_same_v<ColumnCheck, winrt::hstring>)
				return getHString(p_index);
#endif
			else
				static_assert(false, "This Type can't be Indexed in Given ROW");
		}
		template <typename ColumnType>
		inline auto at(const Index p_index) noexcept;
		template <typename ColumnType>
		inline auto get() noexcept
		{
			return get<ColumnType>(m_get_cur_index);
		}
		template <typename ColumnType>
		inline auto at() noexcept
		{
			return at<ColumnType>(m_get_cur_index);
		}

		int getNoOfColumns() noexcept;

		// Ensures that this function is for those types that
		// Do not take a reference as parameter
		template <typename Value, typename = std::enable_if_t<!std::is_reference_v<Value>>>
		PrepareStatement& bind(const Value p_val)
		{
			return bind(m_bind_cur_index, p_val);
		}
		// Ensures that this function is for those types that
		// Take a reference as parameter
		template <typename Value, typename = std::enable_if_t<std::is_reference_v<Value>>>
		PrepareStatement& bind(const Value& p_val)
		{
			return bind(m_bind_cur_index, p_val);
		}

		PrepareStatement& bind(const Index p_index, const std::string_view p_value);
		PrepareStatement& bind(const Index p_index, const std::wstring_view p_value);
		PrepareStatement& bind(const Index p_index, const double p_value);
		PrepareStatement& bind(const Index p_index, const std::int32_t p_value);
		PrepareStatement& bind(const Index p_index, const DataTypes::Int64 p_value);
#ifdef TUESL_USING_CPP_WINRT
		PrepareStatement& bind(const Index p_index, const TimeSpan& p_value);
		PrepareStatement& bind(const Index p_index, const DateTime& p_value);
#endif
		PrepareStatement& bind(const Index p_index, const std::nullptr_t);

		PrepareStatement& bind(const std::string_view p_index, const std::nullptr_t);
		// Provide Index to Bind in the form of a String
		template <typename Value>
		PrepareStatement& bind(const std::string_view p_index, const Value& val);
	};

	template <typename T>
	inline auto PrepareStatement::at(const Index p_index) noexcept
	{
		return get<T>(p_index);
	}

	// Provide Index to Bind in the form of a String
	template <typename Value>
	PrepareStatement& PrepareStatement::bind(const std::string_view p_index,
														  const Value&				 val)
	{
		return bind(convertStringIndexToPosition(p_index), val);
	}
} // namespace TUESL::SQLite
