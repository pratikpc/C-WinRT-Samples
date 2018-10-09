#pragma once

#if __has_include("winrt/Windows.Web.Http.h")
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

	struct WebClient
	{
	 private:
#ifdef TUESL_USING_CPP_WINRT
		HttpClient m_web_client;
#endif

	 public:
#ifdef TUESL_USING_CPP_WINRT
		void setUserAgent(const std::wstring_view p_user_agent);

		void addHeader(const std::wstring_view p_key, const std::wstring_view p_value);
		void removeHeader(const std::wstring_view p_key);

		auto deleteAsync(const Uri& p_uri);
		auto deleteAsync(const std::wstring_view p_uri);

		auto getStringAsync(const Uri& p_uri);
		auto getStringAsync(const std::wstring_view p_uri);

		auto getAsync(const Uri& p_uri);
		auto getAsync(const std::wstring_view p_uri);

		IAsyncOperation<hstring> ReadJsonFromUriAsync(const std::wstring_view p_uri);
#endif
	};
} // namespace TUESL::Net