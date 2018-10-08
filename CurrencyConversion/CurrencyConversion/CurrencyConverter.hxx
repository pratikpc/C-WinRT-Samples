#pragma once

// Required for Manipulating SQLite
#include <TUESL/SQLite/Database.hxx>
#include <TUESL/SQLite/PrepareStatement.hxx>

// Required for Accessing Internet via the web
#include <TUESL/Net/NetUtility.hxx>

// Required to Manipulate JSON
#include <winrt/Windows.Data.Json.h>

// Required to deal with CoRoutines
#include <winrt/Windows.Foundation.h>
// Also Required to Deal with CoRoutines that Yield
#include <experimental/generator>

// Required to Access Local Folder
#include <winrt/Windows.Storage.h>

namespace Currency
{
	namespace
	{
		using namespace winrt;

		using Windows::Foundation::DateTime;
		using Windows::Foundation::TimeSpan;

		using Windows::Foundation::IAsyncAction;
		using Windows::Foundation::IAsyncOperation;

		using Windows::Data::Json::JsonObject;
		using Windows::Data::Json::JsonValue;

		using Windows::UI::Core::CoreDispatcher;
		using Windows::UI::Core::CoreWindow;

		using namespace std::string_literals;

		using namespace TUESL::SQLite;

		using std::experimental::generator;
	} // namespace

	// Unnamed namespace kept to ensure that access is limited to
	// CurrencyConverter Class only
	namespace
	{
		namespace CurrencyJsonAPIURLs
		{
			constexpr const auto URL_CURRENCY_IDs =
				 L"https://free.currencyconverterapi.com/api/v6/currencies";

			// Example URI For Conversion
			// https://free.currencyconverterapi.com/api/v6/convert?q={from}_{to}&compact=ultra
			constexpr const auto URL_CURRENCY_AMTs =
				 L"https://free.currencyconverterapi.com/api/v6/convert?compact=ultra&q=";
		} // namespace CurrencyJsonAPIURLs
		namespace TableNames
		{
			constexpr const auto TABLE_CURRENCY_IDs	 = "TABLE_CURRENCY_IDs";
			constexpr const auto TABLE_CURRENCY_VALUES = "TABLE_CURRENCY_VALUES";
		} // namespace TableNames
		namespace ColumnNames
		{
			namespace CurrencyIDs
			{
				constexpr const auto COLUMN_ID	  = "id";
				constexpr const auto COLUMN_NAME	= "currencyName";
				constexpr const auto COLUMN_SYMBOL = "currencySymbol";
			} // namespace CurrencyIDs
			namespace CurrencyValues
			{
				constexpr const auto COLUMN_FROM				 = "from_col";
				constexpr const auto COLUMN_TO				 = "to_col";
				constexpr const auto COLUMN_AMT_CONVERSION = "amt_col";
				constexpr const auto COLUMN_TIME				 = "time_col";
			} // namespace CurrencyValues
		}	 // namespace ColumnNames

	} // namespace

	struct CurrencyConverter
	{
	 private:
		Database m_db{""};

	 private:
		void CreateTableCurrencyIDs();
		void InsertIntoCurrencyIDs(JsonObject p_results);

		int  GetCountOfCurrencyIDs();
		bool HasCurrencyValuesPresent();

		void CreateTableCurrencyValues();
		void InsertCurrencyValue(const hstring p_from_code,
										 const hstring p_to_code,
										 const double  p_converted_value);

	 public:
		IAsyncAction SetupTableCurrencyIDs();

		generator<hstring> GetAllCurrencyNamesAsync();

		hstring GetCurrencyIDFromName(const hstring p_currency_name);
		hstring GetCurrencySymbolFromName(const hstring p_currency_name);

		std::pair<hstring, hstring> GetLatestConversionOperation();

		hstring GetCurrencyNameFromID(const hstring p_currency_name);

		IAsyncOperation<double> GetConvertedCurrencyValue(const hstring p_from_code,
																		  const hstring p_to_code);

		void DeleteAllCurrencyValuesOlderThanTime(const TimeSpan& p_time);

	 public:
		CurrencyConverter();
	};
} // namespace Currency