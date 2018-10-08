#include "pch.h"
#include "App.h"
#include "MainPage.h"

namespace winrt::CurrencyConversion::implementation
{
	App::App()
	{
		InitializeComponent();
		Suspending({this, &App::OnSuspending});
	}
	void App::OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&) {}
	void App::OnNavigationFailed(IInspectable const&,
													NavigationFailedEventArgs const& e)
	{
		throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
	}
} // namespace winrt::CurrencyConversion::implementation