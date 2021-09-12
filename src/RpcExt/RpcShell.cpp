#include "stdafx.h"
#include "RpcShell.h"

#include "RpcExt.h"
#include "InterfaceMap.h"

RpcExt _rpcExt;

HRESULT RE_API __stdcall RE_Initialize(DWORD dwCoInit)
{
	HRESULT hr = CoInitializeEx(nullptr, dwCoInit);
	if (SUCCEEDED(hr))
	{
		hr = _rpcExt.Initialize(dwCoInit);
	}

	return hr;
}

void RE_API __stdcall RE_Uninitialize()
{
	_rpcExt.Uninitialize();

	CoUninitialize();
}

HRESULT RE_API __stdcall RE_OleInitialize()
{
	HRESULT hr = OleInitialize(nullptr);
	if (SUCCEEDED(hr))
	{
		hr = _rpcExt.Initialize(COINIT_APARTMENTTHREADED);
	}

	return hr;
}

void RE_API __stdcall RE_OleUninitialize()
{
	_rpcExt.Uninitialize();

	OleUninitialize();
}

bool _IsLocalhost(LPCWSTR serverName)
{
	return !serverName || *serverName == '\0' || _wcsicmp(serverName, L".") == 0 || _wcsicmp(serverName, L"localhost") == 0 || _wcsicmp(serverName, L"127.0.0.1") == 0;
}

HRESULT RE_API __stdcall RE_CreateInstance(__in REFCLSID rclsid, __in_opt LPUNKNOWN pOUnkOuter, __in DWORD clsContext,
	__in RE_CONNECTINFO* pConnInfo, __in REFIID riid, __out void** ppv)
{
	//AtlTrace("RE_CreateInstance\n");

	if (!ppv)
		return E_POINTER;

	if (pConnInfo == nullptr || (pConnInfo->port == 0 && pConnInfo->appName == nullptr))
		return CoCreateInstance(rclsid, pOUnkOuter, clsContext, riid, ppv);

	if ((pConnInfo->options & RE_CF_ALWAYS_EX) == 0 && _IsLocalhost(pConnInfo->serverName))
		return CoCreateInstance(rclsid, pOUnkOuter, clsContext, riid, ppv);

	HRESULT hr = _rpcExt.Initialize(-1);
	if (FAILED(hr))
		return hr;

	CComPtr<ProxyManager> spProxyManager;
	hr = ProxyManager::_CreateInstance(__uuidof(ProxyManager), (void**)&spProxyManager);
	if (SUCCEEDED(hr))
	{
		hr = spProxyManager->Connect(rclsid, pConnInfo, riid, ppv);
	}
	else
	{
		_rpcExt.Uninitialize();
	}

	return hr;
}

HRESULT RE_API __stdcall RE_RegisterClassObject(__in REFCLSID rclsid, __in LPUNKNOWN pUnk, __in DWORD clsContext,
	__in DWORD regFlags, __in DWORD usage, __out LPDWORD pRegId)
{
	return _rpcExt.m_classObjectMap.Insert(rclsid, pUnk, clsContext, regFlags, usage, pRegId);
}

HRESULT RE_API __stdcall RE_RevokeClassObject(__in DWORD regId)
{
	return _rpcExt.m_classObjectMap.Remove(regId);
}

HRESULT RE_API __stdcall RE_Listen(__in DWORD port)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RE_Listen : port : %d", port);

	HRESULT hr = _rpcExt.m_rpcServer.Startup(port);

	return hr;
}

HRESULT RE_API __stdcall RE_Listen(__in LPCWSTR appName)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"RE_Listen : appName : %ws", appName);

	return _rpcExt.m_rpcServer.Startup(appName);
}

HRESULT RE_API __stdcall RE_StopListening()
{
	HRESULT hr = _rpcExt.m_rpcServer.Stop();

	return 0;
}

static WCHAR _rpcKeyName[] = L"Software\\Keymnt\\RpcExt";

HRESULT RE_API __stdcall RE_GetRegKeyName(__in LPCWSTR appName, __out LPWSTR path, __out DWORD size)
{
	if (!appName)
		return E_INVALIDARG;

	int n1 = wcslen(_rpcKeyName);
	int n2 = wcslen(appName);

	int n = n1 + 1 + n2;
	if (size <= n)
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

	wcscpy_s(path, size, _rpcKeyName);
	path[n1] = '\\';
	wcscpy_s(path + n1 + 1, size - n1 - 1, appName);

	return S_OK;
}

HRESULT RE_API __stdcall RE_RegisterTypeLib(REFGUID libid)
{
	return _interfaceMap.LoadInterfaces(libid);
}

HRESULT RE_API __stdcall RE_RegisterAccessChannel(__in REFGUID appId, __in REFGUID typeId, __in IScAccessChannel* pChannel)
{
	return _rpcExt.m_appContextMap.RegisterChannel(appId, typeId, pChannel);
}

HRESULT RE_API __stdcall RE_RevokeAccessChannel(__in REFGUID appId, __in REFGUID typeId)
{
	return _rpcExt.m_appContextMap.RevokeChannel(appId, typeId);
}

HRESULT RE_API __stdcall RE_GetCallContext(__out IRpcCallContext** ppContext)
{
	RpcThreadInfo* pThreadInfo = nullptr;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(&pThreadInfo);
	if (hr == S_OK)
	{
		RpcCallContext* pContext = pThreadInfo->GetContext();
		if (pContext)
		{
			hr = pContext->QueryInterface(__uuidof(IRpcCallContext), (void**)ppContext);
		}
		else
		{
			hr = S_FALSE;
		}

		pThreadInfo->Release();
	}

	return hr;
}

HRESULT RE_API __stdcall RE_GetAccessToken(__out IScAccessToken** ppToken)
{
	if (!ppToken)
		return E_POINTER;

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(&spThreadInfo);
	if (hr == S_OK)
	{
		RpcCallContext* pContext = spThreadInfo->GetContext();
		if (pContext)
		{
			hr = pContext->GetAccessToken(ppToken);
		}
		else
		{
			hr = S_FALSE;
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RE_GetAccessToken - GetAccessToken, hr : #%08x", hr);
		}
	}
	else
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RE_GetAccessToken - GetThreadInfo, hr : #%08x", hr);
	}

	return hr;
}

HRESULT RE_API __stdcall RE_GetAccessToken(__in REFIID riid, __out void** ppv)
{
	if (!ppv)
		return E_POINTER;

	CComPtr<IScAccessToken> spToken;
	HRESULT hr = RE_GetAccessToken(&spToken);
	if (hr == S_OK)
	{
		hr = spToken->QueryInterface(riid, ppv);
	}

	return hr;
}

HRESULT RE_API __stdcall RE_AdviseConnection(IUnknown* pObj, IRpcConnectionSink* pSink, BOOL addRef)
{
	CComQIPtr<IRpcProxyInfo> spInfo = pObj;
	if (spInfo)
	{
		CComPtr<IRpcConnection> spConn;
		HRESULT hr = spInfo->GetConnection(__uuidof(IRpcConnection), (void**)&spConn);
		if (hr == S_OK)
		{
			spConn->Advise(pSink, FALSE);
		}

		return hr;
	}

	return E_NOINTERFACE;
}

HRESULT RE_API __stdcall RE_UnadviseConnection(IUnknown* pObj, __in IRpcConnectionSink* pSink)
{
	CComQIPtr<IRpcProxyInfo> spInfo = pObj;
	if (spInfo)
	{
		CComPtr<IRpcConnection> spConn;
		HRESULT hr = spInfo->GetConnection(__uuidof(IRpcConnection), (void**)&spConn);
		if (hr == S_OK)
		{
			spConn->Unadvise(pSink);
		}

		return hr;
	}

	return E_NOINTERFACE;
}

HRESULT RE_API __stdcall RE_HealthCheck(IUnknown* pObj)
{
	CComQIPtr<IRpcProxyInfo> spInfo = pObj;
	if (spInfo)
	{
		CComPtr<IRpcClientConnection> spConn;
		HRESULT hr = spInfo->GetConnection(__uuidof(IRpcClientConnection), (void**)&spConn);
		if (hr == S_OK)
		{
			return spConn->HealthCheck();
		}
	}

	return RPC_E_DISCONNECTED;
}

HRESULT RE_API __stdcall RE_GetLastConnectResult(IUnknown* pObj)
{
	CComQIPtr<IRpcProxyInfo> spInfo = pObj;
	if (spInfo)
	{
		CComPtr<IRpcClientConnection> spConn;
		HRESULT hr = spInfo->GetConnection(__uuidof(IRpcClientConnection), (void**)&spConn);
		if (hr == S_OK)
		{
			return spConn->GetLastConnectResult();
		}
	}

	return E_UNEXPECTED;
}
