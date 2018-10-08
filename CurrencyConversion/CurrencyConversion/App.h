//
// Declaration of the App class.
//
#pragma once

#include "App.g.h"
#include "App.xaml.g.h"

namespace winrt::CurrencyConversion::implementation
{
	namespace
	{
		using namespace Windows::UI::Xaml::Navigation;
	}

	struct App : public AppT<App>
	{
		App();

		void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const& e)
		{
			using namespace winrt;
			using namespace Windows::ApplicationModel::Activation;
			using Windows::UI::Xaml::Window;
			using Windows::UI::Xaml::Controls::Frame;

			Frame rootFrame = nullptr;

			auto content = Window::Current().Content();
			if (content)
			{
				rootFrame = content.try_as<Frame>();
			}

			// Do not repeat app initialization when the Window already has content,
			// just ensure that the window is active
			if (rootFrame == nullptr)
			{
				// Create a Frame to act as the navigation context and associate it with
				// a SuspensionManager key
				rootFrame = Frame();

				rootFrame.NavigationFailed({this, &App::OnNavigationFailed});

				if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated)
				{
					// Restore the saved session state only when appropriate, scheduling the
					// final launch steps after the restore is complete
				}

				if (e.PrelaunchActivated() == false)
				{
					if (rootFrame.Content() == nullptr)
					{
						// When the navigation stack isn't restored navigate to the first page,
						// configuring the new page by passing required information as a navigation
						// parameter
						rootFrame.Navigate(xaml_typename<MainPage>(), box_value(e.Arguments()));
					}
					// Place the frame in the current Window
					Window::Current().Content(rootFrame);
					// Ensure the current window is active
					Window::Current().Activate();
				}
			}
			else
			{
				if (e.PrelaunchActivated() == false)
				{
					if (rootFrame.Content() == nullptr)
					{
						// When the navigation stack isn't restored navigate to the first page,
						// configuring the new page by passing required information as a navigation
						// parameter
						rootFrame.Navigate(xaml_typename<MainPage>(), box_value(e.Arguments()));
					}
					// Ensure the current window is active
					Window::Current().Activate();
				}
			}
		}

		void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
		void OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const&);
	};
} // namespace winrt::CurrencyConversion::implementation