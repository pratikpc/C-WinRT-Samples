// Includes the pch file
// This is known to boost compilation speeds
#include "pch.h"

#include "CurrencyConverter.hxx"

namespace Currency
{
	void CurrencyConverter::CreateTableCurrencyIDs()
	{
		const std::string sql =
			 "CREATE TABLE IF NOT EXISTS "s + TableNames::TABLE_CURRENCY_IDs + " ("s +
			 ColumnNames::CurrencyIDs::COLUMN_ID + " BLOB NOT NULL,"s +
			 ColumnNames::CurrencyIDs::COLUMN_NAME + " BLOB NOT NULL, "s +
			 ColumnNames::CurrencyIDs::COLUMN_SYMBOL + " BLOB NOT NULL"s + ");"s;

		// Create Table
		m_db.executeSQL(sql);
	}
	void CurrencyConverter::CreateTableCurrencyValues()
	{
		const std::string sql =
			 "CREATE TABLE IF NOT EXISTS "s + TableNames::TABLE_CURRENCY_VALUES + " ("s +
			 ColumnNames::CurrencyValues::COLUMN_FROM + " BLOB NOT NULL,"s +
			 ColumnNames::CurrencyValues::COLUMN_TO + " BLOB NOT NULL,"s +
			 ColumnNames::CurrencyValues::COLUMN_AMT_CONVERSION + " REAL NOT NULL,"s +
			 ColumnNames::CurrencyValues::COLUMN_TIME + " REAL NOT NULL"s + ");"s;

		// Create Table
		m_db.executeSQL(sql);
	}
	void CurrencyConverter::InsertCurrencyValue(const hstring p_from_code,
															  const hstring p_to_code,
															  const double  p_converted_value)
	{
		// Get the Current Time Value
		const DateTime current_time = winrt::clock::now();

		PrepareStatement ps{};
		// Ensure that the code is Present between a Begin And End Transaction
		// Helps Raise Performance

		m_db.transactionBegin();

		const std::string sql =
			 "INSERT OR IGNORE INTO "s + TableNames::TABLE_CURRENCY_VALUES + "(" +
			 ColumnNames::CurrencyValues::COLUMN_FROM + ","s +
			 ColumnNames::CurrencyValues::COLUMN_TO + ","s +
			 ColumnNames::CurrencyValues::COLUMN_AMT_CONVERSION + ","s +
			 ColumnNames::CurrencyValues::COLUMN_TIME + ") VALUES(?,?,?,?)"s;

		ps.prepare(m_db, sql);
		ps.bind(p_from_code);
		ps.bind(p_to_code);
		ps.bind(p_converted_value);
		ps.bind(current_time);
		ps.execute();

		// Note that if 1 USD = 70 INR
		// Then We Find 1 INR = 1/70 USD

		ps.prepare(m_db, sql);
		ps.bind(p_to_code);
		ps.bind(p_from_code);
		ps.bind(1 / p_converted_value);
		ps.bind(current_time);
		ps.execute();

		// End the Transaction
		// Ensure changes are committed to database
		m_db.transactionEnd();
	}
	IAsyncOperation<double>
		 CurrencyConverter::GetConvertedCurrencyValue(const hstring p_from_code,
																	 const hstring p_to_code)
	{
		// First check within SQLite Database If the Value has been already added
		// If Not, then fire a Json Query

		{
			const std::string sql =
				 "SELECT "s + ColumnNames::CurrencyValues::COLUMN_AMT_CONVERSION + " FROM " +
				 TableNames::TABLE_CURRENCY_VALUES + " WHERE " +
				 ColumnNames::CurrencyValues::COLUMN_FROM + "=?"s + " AND " +
				 ColumnNames::CurrencyValues::COLUMN_TO + "=?"s;
			PrepareStatement ps{};

			ps.prepare(m_db, sql);

			ps.bind(p_from_code);
			ps.bind(p_to_code);

			// if Currency Value present
			if (ps.hasNext())
			{
				const std::optional<double> convert_val = ps.get<double>();
				if (convert_val.has_value())
					co_return convert_val.value();
			}
		}

		// As it was not found in SQLite Database, firing Json Query
		{

			const hstring code_from_to_concatenated = p_from_code + L"_" + p_to_code;

			const hstring uri =
				 CurrencyJsonAPIURLs::URL_CURRENCY_AMTs + code_from_to_concatenated;

			const hstring json = co_await m_web_client.ReadJsonFromUriAsync(uri);

			if (std::empty(json))
				co_return 0.0;

			// Convert Read Json String to Value Object
			const auto json_obj = JsonObject::Parse(json);

			// Note that data is in the form
			// {"USD_INR" : 45}
			const double converted_amt =
				 (json_obj.GetNamedNumber(code_from_to_concatenated));

			// Add this currency value with time stamp
			InsertCurrencyValue(p_from_code, p_to_code, converted_amt);
			co_return converted_amt;
		}
	}
	void CurrencyConverter::DeleteAllCurrencyValuesOlderThanTime(const TimeSpan& p_time)
	{
		const std::string sql = "DELETE FROM "s + TableNames::TABLE_CURRENCY_VALUES +
										" WHERE "s + ColumnNames::CurrencyValues::COLUMN_TIME +
										" < ?";

		PrepareStatement ps;

		ps.prepare(m_db, sql);
		ps.bind(p_time);
		ps.execute();
	}
	void CurrencyConverter::InsertIntoCurrencyIDs(const JsonObject p_results)
	{

		// Ensure that the code is Present between a Begin And End Transaction
		// Helps Raise Performance

		m_db.transactionBegin();

		const std::string sql = "INSERT OR IGNORE INTO "s + TableNames::TABLE_CURRENCY_IDs +
										"("s + ColumnNames::CurrencyIDs::COLUMN_ID + ","s +
										ColumnNames::CurrencyIDs::COLUMN_NAME + ","s +
										ColumnNames::CurrencyIDs::COLUMN_SYMBOL +
										") VALUES(?,?,?);"s;

		PrepareStatement ps{};
		for (const auto it = p_results.First(); it.HasCurrent(); it.MoveNext())
		{
			// This is a PrepareStatement
			// Creates the Statement to be executed
			ps.prepare(m_db, sql);

			const auto obj = it.Current().Value().GetObject();
			if (obj.HasKey(to_hstring(ColumnNames::CurrencyIDs::COLUMN_ID)))
			{
				const auto id =
					 obj.GetNamedString(to_hstring(ColumnNames::CurrencyIDs::COLUMN_ID));
				ps.bind(id);
			}
			else
			{
				ps.bind("");
			}

			if (obj.HasKey(to_hstring(ColumnNames::CurrencyIDs::COLUMN_NAME)))
			{
				const auto name =
					 obj.GetNamedString(to_hstring(ColumnNames::CurrencyIDs::COLUMN_NAME));
				ps.bind(name);
			}
			else
			{
				ps.bind("");
			}

			if (obj.HasKey(to_hstring(ColumnNames::CurrencyIDs::COLUMN_SYMBOL)))
			{
				const auto symbol =
					 obj.GetNamedString(to_hstring(ColumnNames::CurrencyIDs::COLUMN_SYMBOL));
				ps.bind(symbol);
			}
			else
			{
				ps.bind("");
			}

			// Runs an Update
			// This Edits the Database
			ps.execute();
		}

		// End the Transaction
		// Ensure changes are committed to database
		m_db.transactionEnd();
	}
	int CurrencyConverter::GetCountOfCurrencyIDs()
	{
		const std::string sql = "SELECT COUNT(*) FROM "s + TableNames::TABLE_CURRENCY_IDs;

		PrepareStatement ps;
		ps.prepare(m_db, sql);
		if (ps.hasNext())
		{
			const auto count = ps.get<int>().value_or(0);
			// Return Number of Elements
			// If count returned is negative, return 0
			return (count < 0) ? 0 : count;
		}
		else
		{
			return 0;
		}
	}
	bool CurrencyConverter::HasCurrencyValuesPresent()
	{
		// Function Returns 0 When No Values present
		const auto no_of_values = GetCountOfCurrencyIDs();
		return no_of_values != 0;
	}
	IAsyncAction CurrencyConverter::SetupTableCurrencyIDs()
	{
		// Setup Table if it doesn't exist
		CreateTableCurrencyIDs();

		const auto has_values = HasCurrencyValuesPresent();

		// If Values are Present, No Need to do anything
		if (has_values)
			co_return;

		// Read Json
		const hstring json = co_await m_web_client.ReadJsonFromUriAsync(
			 CurrencyJsonAPIURLs::URL_CURRENCY_IDs);

		if (std::empty(json))
			co_return;

		// Convert it Into a JSON Object
		const auto json_object = JsonObject::Parse(json);

		// Note that data format contains object in form "result"
		const auto results = json_object.GetNamedObject(L"results");

		InsertIntoCurrencyIDs(results);
	}
	inline void CurrencyConverter::SetupWebClient()
	{
		// Set User Agent to Microsoft Edge
		m_web_client.setUserAgent(
			 L"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like "
			 L"Gecko) "
			 L"Chrome/39.0.2171.71 "
			 L"Safari/537.36 Edge/12.0");

		// Set the Header to only provide Json Values
		m_web_client.addHeader(L"accept", L"application/json");
	}
	void CurrencyConverter::SetupDatabase()
	{
		// Add Path to Cache Directory
		const hstring cache_folder_path =
			 Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path();

		const std::string database_path =
			 to_string(cache_folder_path) + "\\" + DATABASE_NAME;

		// Open the Database
		m_db.open(database_path);
	}
	generator<hstring> CurrencyConverter::GetAllCurrencyNamesAsync()
	{
		const std::string sql = "SELECT "s + ColumnNames::CurrencyIDs::COLUMN_NAME +
										" FROM "s + TableNames::TABLE_CURRENCY_IDs + " ORDER BY " +
										ColumnNames::CurrencyIDs::COLUMN_NAME;

		PrepareStatement ps;
		ps.prepare(m_db, sql);

		while (ps.hasNext())
		{
			const auto currency_name = ps.get<hstring>().value_or(L"");
			// Return All Currency Names found
			co_yield currency_name;
		}
	}
	hstring CurrencyConverter::GetCurrencyIDFromName(const hstring p_currency_name)
	{
		const std::string sql = "SELECT "s + ColumnNames::CurrencyIDs::COLUMN_ID +
										" FROM "s + TableNames::TABLE_CURRENCY_IDs + " WHERE "s +
										ColumnNames::CurrencyIDs::COLUMN_NAME + "=?"s;
		PrepareStatement ps;
		ps.prepare(m_db, sql);
		ps.bind(p_currency_name);

		if (ps.hasNext())
			return ps.get<hstring>().value_or(L"");
		else
			return L"";
	}
	hstring CurrencyConverter::GetCurrencySymbolFromName(const hstring p_currency_name)
	{
		const std::string sql = "SELECT "s + ColumnNames::CurrencyIDs::COLUMN_SYMBOL +
										" FROM "s + TableNames::TABLE_CURRENCY_IDs + " WHERE "s +
										ColumnNames::CurrencyIDs::COLUMN_NAME + "=?"s;
		PrepareStatement ps;
		ps.prepare(m_db, sql);
		ps.bind(p_currency_name);

		if (ps.hasNext())
			return ps.get<hstring>().value_or(L"");
		else
			return L"";
	}

	std::pair<hstring, hstring> CurrencyConverter::GetLatestConversionOperation()
	{
		// Query taken from https://stackoverflow.com/a/19268554
		// See example Query
		// select * from your_table where product_price = (SELECT max(product_price) FROM
		// your_table)
		const std::string sql = "SELECT "s + ColumnNames::CurrencyValues::COLUMN_FROM +
										","s + ColumnNames::CurrencyValues::COLUMN_TO + " FROM "s +
										TableNames::TABLE_CURRENCY_VALUES + " WHERE "s +
										ColumnNames::CurrencyValues::COLUMN_TIME + " =("s +
										" SELECT "s + "MAX(" +
										ColumnNames::CurrencyValues::COLUMN_TIME + ") FROM " +
										TableNames::TABLE_CURRENCY_VALUES + ");"s;

		PrepareStatement ps;
		ps.prepare(m_db, sql);

		if (ps.hasNext())
		{
			const auto from_code = ps.get<hstring>().value_or(L"");
			const auto to_code	= ps.get<hstring>().value_or(L"");

			return std::make_pair(from_code, to_code);
		}
		else
		{
			return std::make_pair(L"", L"");
		}
	}

	hstring CurrencyConverter::GetCurrencyNameFromID(const hstring p_currency_id)
	{
		const std::string sql = "SELECT "s + ColumnNames::CurrencyIDs::COLUMN_NAME +
										" FROM "s + TableNames::TABLE_CURRENCY_IDs + " WHERE "s +
										ColumnNames::CurrencyIDs::COLUMN_ID + "=?"s;
		PrepareStatement ps;
		ps.prepare(m_db, sql);
		ps.bind(p_currency_id);

		if (ps.hasNext())
			return ps.get<hstring>().value_or(L"");
		else
			return L"";
	}

	CurrencyConverter::CurrencyConverter()
	{
		// Verify if threading is enabled within database
		// Throw Exception if Not
		assert(Database::IsThreadingEnabled() && "Threading is Disabled within SQLite");

		SetupWebClient();
		SetupDatabase();

		CreateTableCurrencyValues();
	}
} // namespace Currency