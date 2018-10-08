#include "pch.h"
#include <TUESL/Net/NetUtility.hxx>

namespace TUESL::Net
{
#ifdef TUESL_USING_CPP_WINRT
	IAsyncOperation<hstring> ReadJsonFromUriAsync(const hstring p_uri)
	{
		HttpClient web_client;

		// Set User Agent to Microsoft Edge
		web_client.DefaultRequestHeaders().UserAgent().TryParseAdd(
			L"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) "
			L"Chrome/39.0.2171.71 "
			L"Safari/537.36 Edge/12.0");
		// Request Json headers
		web_client.DefaultRequestHeaders().TryAppendWithoutValidation(L"accept",
			L"application/json");

		try
		{
			const auto http_response_msg = co_await web_client.GetAsync(Uri{ p_uri });

			// If the Status is Not Okay
			// Assume Query Failed
			if (http_response_msg.StatusCode() != HttpStatusCode::Ok)
			{
				co_return L"";
			}

			// Read the JSON String and Store it
			const hstring json = co_await http_response_msg.Content().ReadAsStringAsync();

			co_return json;
		}
		catch (...)
		{
			co_return L"";
		}
	}
#endif
}