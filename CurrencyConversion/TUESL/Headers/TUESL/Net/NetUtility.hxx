#pragma once

#if __has_include("winrt/Windows.Web.Http.Headers.h")
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
// Required to Deal with Internet Connections
#	include <winrt/Windows.Web.Http.Headers.h>
#	include <winrt/Windows.Web.Http.h>

// Required to deal with CoRoutines
#	include <winrt/Windows.Foundation.h>
#endif

namespace TUESL::Net
{
	namespace
	{
#ifdef TUESL_USING_CPP_WINRT
		using namespace winrt;

		using Windows::Foundation::Uri;
		using Windows::Web::Http::HttpClient;
		using Windows::Web::Http::HttpResponseMessage;
		using Windows::Web::Http::HttpStatusCode;

		using Windows::Foundation::IAsyncOperation;
#endif
	} // namespace

#ifdef TUESL_USING_CPP_WINRT
	IAsyncOperation<hstring> ReadJsonFromUriAsync(const hstring p_uri);
#endif
} // namespace TUESL::Net