#include "pch.h"

#include "MainPage.h"

namespace winrt::CurrencyConversion::implementation
{
	IVector<IInspectable> MainPage::CurrencyNameList() const
	{
		return m_currency_list;
	}
	fire_and_forget MainPage::AddValuesToCurrencyIDList()
	{
		MessageInfo().Text(L"Obtaining List of Currencies");
		
		co_await m_currency_converter.SetupTableCurrencyIDs();
		
		CurrencyNameList().Clear();
		// This will Update Both Drop Down Lists with Currency Names
		auto currency_names_list = m_currency_converter.GetAllCurrencyNamesAsync();
		for (const hstring& currency_name : currency_names_list)
			if (!std::empty(currency_name))
				CurrencyNameList().Append(box_value(currency_name));

		// Check if No Currency Name Element was Loaded
		// If true, Display Error Message and Exit
		if (CurrencyNameList().Size() == 0)
		{
			ContentDialog error_diag{};
			error_diag.Title(winrt::box_value(L"Currency Converter"));
			error_diag.Content(winrt::box_value(
				 L"Error Occured: Unable to Load Data. Please check if Internet is On. "
				 L"Restart Application when started"));
			error_diag.CloseButtonText(L"OK");
			co_await error_diag.ShowAsync();
		}
		else
		{
			MessageInfo().Text(L"");
		}
	}

	void MainPage::CleanupDatabaseOfOldCurrencyConversionsInFixTimePeriod()
	{
		// Note that we cache all the Data within the SQLite Database
		// This is to ensure that we do not query the web service excessively
		// As such we shall ensure that all values are updated in every
		// Fixed time period
		// For this we will have to register a Timer
		// For further reference, please see
		// https://docs.microsoft.com/en-us/uwp/api/windows.system.threading.threadpooltimer

		using std::chrono::duration_cast;

		const auto cleanup_currency = [this](const auto&) {
			const auto current_time = winrt::clock::now().time_since_epoch();

			// Note that we have set the Offset to 12 Hours
			// As such any data prior to 12 hours shall be deleted
			constexpr const auto offset = duration_cast<TimeSpan>(std::chrono::hours{12});

			// The difference in current time and delete_prior tells us exactly in which
			// duration to delete
			const auto delete_prior = current_time - offset;

			m_currency_converter.DeleteAllCurrencyValuesOlderThanTime(delete_prior);
		};

		// Let us set it to Reset Every 2 Hours
		constexpr const auto reset_duration =
			 duration_cast<TimeSpan>(std::chrono::hours{2});

		// Create the ThreadPoolTimer here
		// This timer will only run every 2 hours
		auto periodic_cleanup_old_currency_timer =
			 ThreadPoolTimer::CreatePeriodicTimer(cleanup_currency, reset_duration);

		// Run CleanUp Currency
		// Ensure it is called at least once
		cleanup_currency(nullptr /*The Passed argument is ignored*/);
	}

	IAsyncAction MainPage::UpdateReadingsAsync()
	{
		const auto src_amt_str = winrt::to_string(FromAmt().Text());

		// If the Source Amount String is empty
		// Do nothing
		if (std::empty(src_amt_str))
			co_return;

		// Get the From and To Codes
		const auto from_selected_idx = FromCurrencyList().SelectedIndex();
		const auto to_selected_idx	= ToCurrencyList().SelectedIndex();

		// If From Selected Index is Unknown
		// Or To Selected Index is Unknown
		// Do Nothing
		if (from_selected_idx < 0 || to_selected_idx < 0)
		{
			co_return;
		}
		if (from_selected_idx == to_selected_idx)
		{
			// As both the currencies are the same
			// Their valuations will be same as well
			ToAmt().Text(FromAmt().Text());
			MessageInfo().Text(L"");
			co_return;
		}

		const auto from_selected_cur_name =
			 unbox_value<winrt::hstring>(CurrencyNameList().GetAt(from_selected_idx));
		const auto to_selected_cur_name =
			 unbox_value<winrt::hstring>(CurrencyNameList().GetAt(to_selected_idx));

		// Get the From and To Codes
		const auto from_code =
			 m_currency_converter.GetCurrencyIDFromName(from_selected_cur_name);
		const auto to_code =
			 m_currency_converter.GetCurrencyIDFromName(to_selected_cur_name);

		const double src_amt = std::stod(src_amt_str);

		if (src_amt == 0.0)
		{
			// In case the Amount Provided is Zero
			// Then we can safely assume that the response
			// shall be zero as well
			// Hence
			// Set ToAmt to Zero
			ToAmt().Text(L"0");
			MessageInfo().Text(L"");
			co_return;
		}

		// Display Updating Message
		MessageInfo().Text(L"Updating Information");

		// Set this as PlaceHolder Value till Update Occurs
		ToAmt().Text(L"To");

		{
			// Intensive Calculations need to be performed in the background
			// Rather than on the UI thread
			// As such we Resume Background Process so that we can perform a few
			// Intensive Calculations
			// For more details, Please Check
			// https://stackoverflow.com/questions/49640092/how-to-call-a-method-on-the-gui-thread-in-c-winrt
			co_await resume_background();

			const double converted_amt_unit_1 =
				 // This function returns the monetary converted value for only 1 Unit
				 co_await m_currency_converter.GetConvertedCurrencyValue(from_code, to_code);

			const double converted_amt = converted_amt_unit_1 * src_amt;

			// As it can be assumed the memory intensive calculations are over
			// We can now re-enable the GUI thread
			co_await winrt::resume_foreground(Dispatcher());

			// Change Reading Only if Value is Not 0
			// 0 is used here as an Error Value
			if (converted_amt != 0.0)
			{
				// Set the To Screen to this value
				ToAmt().Text(winrt::to_hstring(converted_amt));
				MessageInfo().Text(L"");
			}
			else
			{
				MessageInfo().Text(L"Error Occurred in obtaining amount");
			}
		}
	}

	fire_and_forget MainPage::Amt_Changed(const IInspectable&, const TextChangedEventArgs&)
	{
		// Regex to Match Numbers
		const std::regex regex{R"(^-?\d+\.?\d*$)"};

		// This stores the number of Matches found
		std::smatch matches;

		// Read the Reading from the Text Box
		const std::string amount_val = to_string(FromAmt().Text());

		// Note that this regex runs and checks
		// If the given input is not a Numeric Value
		// Then reset the value
		if (!(std::regex_match(amount_val, matches, regex) && std::size(matches) == 1))
		{
			// If Not Number,
			// Reset the values to
			// Initial Conditions
			FromAmt().Text(L"");
			ToAmt().Text(L"To");

			co_return;
		}
		co_await UpdateReadingsAsync();
	}

	fire_and_forget MainPage::List_SelectionChanged(const IInspectable&,
																	const SelectionChangedEventArgs&)
	{
		co_await UpdateReadingsAsync();
	}

	MainPage::MainPage() :
		 m_currency_converter{}, m_currency_list{
											  winrt::single_threaded_vector<IInspectable>()}
	{
		InitializeComponent();

		AddValuesToCurrencyIDList();

		CleanupDatabaseOfOldCurrencyConversionsInFixTimePeriod();
	}
} // namespace winrt::CurrencyConversion::implementation