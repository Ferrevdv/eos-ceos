#include "Transport.h"


int getStatusCode(HANDLE hRequest)
{
	DWORD statusCode = 0;
	DWORD statusSize = sizeof(DWORD);
	if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX))
		return 0;
	return statusCode;
}


Parser* makeHTTPRequest(PBYTE bufferIn, UINT32 bufferLen)
{
	HANDLE hSession = NULL;
	HANDLE hConnect = NULL;
	HANDLE hRequest = NULL;

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	DWORD statusCode = 0;

	PVOID respBuffer = NULL;
	DWORD respSize = 0;
	PCHAR pckt = nullptr;
	SIZE_T sizePckt = 0;

	LPWSTR HttpUrl = NULL;
	DWORD HttpUrlLen = 0;

	UCHAR buffer[1024] = { 0 };

	DWORD httpFlags = 0;

	WINHTTP_PROXY_INFO proxyInfo = { 0 };
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig = { 0 };
	WINHTTP_AUTOPROXY_OPTIONS autoProxyOptions = { 0 };

	DWORD accessType = 0;
	LPCWSTR httpProxy = NULL;
	LPCWSTR noProxyBypass = NULL;
	DWORD proxyFlag = 0;

	if (ceosConfig->isProxyEnabled && ceosConfig->proxyURL)
	{
		accessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		httpProxy = ceosConfig->proxyURL;
		noProxyBypass = WINHTTP_NO_PROXY_BYPASS;
	}
	else 
	{
		accessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
		httpProxy = WINHTTP_NO_PROXY_NAME;
		noProxyBypass = WINHTTP_NO_PROXY_BYPASS;
	}

	httpFlags = WINHTTP_FLAG_BYPASS_PROXY_CACHE;
	if (ceosConfig->isSSL)
		httpFlags |= WINHTTP_FLAG_SECURE;


	hSession = WinHttpOpen(ceosConfig->userAgent, accessType, httpProxy, noProxyBypass, proxyFlag);
	if (!hSession)
		return NULL;

	hConnect = WinHttpConnect(hSession, ceosConfig->hostName, ceosConfig->httpPort, 0);
	if (!hConnect)
        {
                WinHttpCloseHandle(hSession);
		return NULL;
        }

	hRequest = WinHttpOpenRequest(hConnect, ceosConfig->httpMethod, ceosConfig->endPoint, NULL, NULL, NULL, httpFlags);
	if (!hRequest)
        {
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
		return NULL;
        }

	if (ceosConfig->isSSL)
	{
		httpFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
			SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
			SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
			SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

		WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &httpFlags, sizeof(DWORD));
	}


	if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
	{
		if (proxyConfig.lpszProxy != NULL && lstrlenW(proxyConfig.lpszProxy) != 0)
		{
			// IE is set to "use a proxy server"
			proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxyInfo.lpszProxy = proxyConfig.lpszProxy;
			proxyInfo.lpszProxyBypass = proxyConfig.lpszProxyBypass;

			if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_PROXY, &proxyInfo, sizeof(proxyInfo)))
                        {
                                WinHttpCloseHandle(hRequest);
                                WinHttpCloseHandle(hConnect);
                                WinHttpCloseHandle(hSession);
				return NULL;
		        }
		}

		else if ((proxyConfig.lpszAutoConfigUrl != NULL && lstrlenW(proxyConfig.lpszAutoConfigUrl) != 0) || proxyConfig.fAutoDetect == TRUE)
		{
			if (proxyConfig.lpszAutoConfigUrl)
			{
				// IE is set to "Use automatic proxy configuration"
				autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
				autoProxyOptions.lpszAutoConfigUrl = proxyConfig.lpszAutoConfigUrl;
				autoProxyOptions.dwAutoDetectFlags = 0;
			}
			else
			{
				// IE is set to "automatically detect settings"
				autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
				autoProxyOptions.lpszAutoConfigUrl = NULL;
				autoProxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
			}

			autoProxyOptions.fAutoLogonIfChallenged = TRUE;
			autoProxyOptions.lpvReserved = NULL;
			autoProxyOptions.dwReserved = 0;

			HttpUrlLen = (15 + lstrlenW(ceosConfig->hostName) + lstrlenW(ceosConfig->endPoint));
			HttpUrl = (LPWSTR)LocalAlloc(LPTR, HttpUrlLen * sizeof(WCHAR));
                        if (HttpUrl == NULL)
                        {
                            WinHttpCloseHandle(hRequest);
                            WinHttpCloseHandle(hConnect);
                            WinHttpCloseHandle(hSession);
                            return NULL;
                        }
			WCHAR fullHttpScheme[20] = L"%ws://%ws:%lu/%ws";

			swprintf_s(HttpUrl, HttpUrlLen, fullHttpScheme, L"http", ceosConfig->hostName, ceosConfig->httpPort, ceosConfig->endPoint);
			
			if (WinHttpGetProxyForUrl(hSession, HttpUrl, &autoProxyOptions, &proxyInfo))
			{
				if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_PROXY, &proxyInfo, sizeof(proxyInfo)))
                                {
                                        LocalFree(HttpUrl);
                                        WinHttpCloseHandle(hRequest);
                                        WinHttpCloseHandle(hConnect);
                                        WinHttpCloseHandle(hSession);
					return NULL;
                                }
			}

                        LocalFree(HttpUrl);

		}

	}

	WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, bufferIn, bufferLen, bufferLen, 0);
	if (!WinHttpReceiveResponse(hRequest, NULL))
	{
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
		return NULL;
	}

	statusCode = getStatusCode(hRequest);

	if (statusCode != 200)
	{
		_err("[HTTP] Status code not OK --> %d\n", statusCode);
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
		return NULL;
	}

	do
	{
		dwSize = 1024;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
		{
			_err("[HTTP] Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                        WinHttpCloseHandle(hRequest);
                        WinHttpCloseHandle(hConnect);
                        WinHttpCloseHandle(hSession);
                        if (respBuffer)
                            LocalFree(respBuffer);
			return NULL;
		}

		if (!WinHttpReadData(hRequest, buffer, 1024, &dwDownloaded))
		{
			_err("[HTTP] Error %u in WinHttpReadData.\n", GetLastError());
                        WinHttpCloseHandle(hRequest);
                        WinHttpCloseHandle(hConnect);
                        WinHttpCloseHandle(hSession);
                        if (respBuffer)
                            LocalFree(respBuffer);
			return NULL;
		}

		respSize += dwDownloaded;

		if (!respBuffer)
			respBuffer = LocalAlloc(LPTR, respSize);
                else
                        respBuffer = LocalReAlloc(respBuffer, respSize, LMEM_MOVEABLE | LMEM_ZEROINIT);

		memcpy((PBYTE)respBuffer + (respSize - dwDownloaded), buffer, dwDownloaded);
		memset(buffer, 0, 1024);

	} while (dwSize > 0);

	
	respBuffer = LocalReAlloc(respBuffer, respSize + 1, LMEM_MOVEABLE | LMEM_ZEROINIT);

	PParser returnParser = newParser((PBYTE)respBuffer, respSize);

	// TODO: free memory (bufferIn, HttpUrl, ...)
	// 'HttpUrl' allocated by LocalAlloc 
            // BUT HttpUrl only allocated in if branch --> free there!
        // Cleanup
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

	return returnParser;


}



Parser* sendAndReceive(PBYTE data, SIZE_T size)
{

#ifdef HTTP_TRANSPORT
	return makeHTTPRequest(data, size);
#endif 

	return nullptr;
}
