//
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include "CurrencyConverter.hxx"

#include <winrt/Windows.System.Threading.h>

#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>

#include <regex>

namespace winrt::CurrencyConversion::implementation
{
	namespace
	{
		using namespace winrt;
		using namespace Windows::Foundation;
		using namespace Windows::Foundation::Collections;
		using namespace Windows::UI::Xaml;
		using namespace Windows::UI::Xaml::Controls;
		using namespace Windows::UI::Core;
		using namespace Windows::UI::ViewManagement;

		using namespace Windows::System::Threading;

		using TUESL::SQLite::Database;
	} // namespace

	struct MainPage : MainPageT<MainPage>
	{
	 private:
		Currency::CurrencyConverter m_currency_converter;
		IVector<IInspectable>		 m_currency_list;

	 public:
		IVector<IInspectable> CurrencyNameList() const;

		void AddValuesToCurrencyIDList();

		void CleanupDatabaseOfOldCurrencyConversionsInFixTimePeriod();

		IAsyncAction UpdateReadingsAsync();

		IAsyncAction Amt_Changed(const IInspectable&, const TextChangedEventArgs&);

		IAsyncAction List_SelectionChanged(const IInspectable&,
													  const SelectionChangedEventArgs&);

		MainPage();
	};
} // namespace winrt::CurrencyConversion::implementation

namespace winrt::CurrencyConversion::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
} // namespace winrt::CurrencyConversion::factory_implementation
