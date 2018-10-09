#include "pch.h"
#include <TUESL/Net/NetUtility.hxx>

namespace TUESL::Net
{
#ifdef TUESL_USING_CPP_WINRT
	void WebClient::setUserAgent(const std::wstring_view p_user_agent)
	{
		m_web_client.DefaultRequestHeaders().UserAgent().TryParseAdd(p_user_agent);
	}
	void WebClient::addHeader(const std::wstring_view p_key,
									  const std::wstring_view p_value)
	{
		m_web_client.DefaultRequestHeaders().Append(p_key, p_value);
	}
	inline void WebClient::removeHeader(const std::wstring_view p_key)
	{
		m_web_client.DefaultRequestHeaders().Remove(p_key);
	}
	inline auto WebClient::deleteAsync(const Uri& p_uri)
	{
		return m_web_client.DeleteAsync(p_uri);
	}
	inline auto WebClient::deleteAsync(const std::wstring_view p_uri)
	{
		return deleteAsync(Uri{p_uri});
	}
	inline auto WebClient::getStringAsync(const Uri& p_uri)
	{
		return m_web_client.GetStringAsync(p_uri);
	}
	inline auto WebClient::getStringAsync(const std::wstring_view p_uri)
	{
		return getStringAsync(Uri{p_uri});
	}
	inline auto WebClient::getAsync(const Uri& p_uri)
	{
		return m_web_client.GetAsync(p_uri);
	}
	inline auto WebClient::getAsync(const std::wstring_view p_uri)
	{
		return getAsync(Uri{p_uri});
	}
	IAsyncOperation<hstring> WebClient::ReadJsonFromUriAsync(const std::wstring_view p_uri)
	{
		try
		{
			// Read the JSON String and Store it
			const hstring json = co_await getStringAsync(p_uri);
			co_return json;
		}
		catch (...)
		{
		}
		co_return L"";
	}
#endif
} // namespace TUESL::Net