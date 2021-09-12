#include "StdAfx.h"
#include "ClassObjectMap.h"

#include "RpcExt.h"
#include "ThreadWindow.h"

ClassObject::ClassObject()
{
	m_threadId = 0;

	m_aptType = APTTYPE_STA;
	m_clsid = CLSID_NULL;

	m_clsContext = CLSCTX_ALL;
	m_regFlags = 0;
	m_register = 0;
	m_usage = 0;

	m_pUnk = nullptr;
	m_pAppContext = nullptr;
}

ClassObject::~ClassObject()
{
	ComApi::Release(&m_pAppContext);	
	ComApi::Release(&m_pUnk);
}

HRESULT ClassObject::Init(REFCLSID rclsid, DWORD clsContext, DWORD flags, DWORD usage, LPUNKNOWN lpUnk)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ClassObject::Init");

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(&spThreadInfo);
	if (FAILED(hr))
		return hr;

	m_threadId = spThreadInfo->m_threadId;
	m_aptType = spThreadInfo->m_aptType;

	m_clsid = rclsid;
	m_clsContext = clsContext;
	m_regFlags = flags;
	m_usage = usage;

	ComApi::Assign(&m_pUnk, lpUnk);

	hr = CoRegisterClassObject(rclsid, lpUnk, clsContext, flags, &m_register);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "ClassObject::Init - RegisterClassObject failed, hr = #%08x", hr);

		return hr;
	}

	GUID appId = GUID_NULL;
	hr = GetAppID(m_clsid, &appId);
	if (SUCCEEDED(hr))
	{
		hr = _rpcExt.m_appContextMap.Insert(appId, &m_pAppContext);

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ClassObject::Init - AppContextMap::Insert, hr = #%08x", hr);
	}
	else
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "ClassObject::Init - GetAppID failed, hr = #%08x", hr);
	}

	return S_OK;
}

HRESULT ClassObject::GetAppID(REFCLSID rclsid, LPGUID lpGuid)
{
	ATLASSERT(lpGuid);

	WCHAR clsid[128];
	if (!StringFromGUID2(rclsid, clsid, 128))
	{
		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ClassObject::GetAppID - StringFromGUID2");

		return E_FAIL;
	}

	WCHAR keyName[MAX_PATH];
	swprintf_s(keyName, MAX_PATH, L"CLSID\\%ws", clsid);

	CRegKey key;
	LONG res = key.Open(HKEY_CLASSES_ROOT, keyName, KEY_READ);
	if (res != ERROR_SUCCESS)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "ClassObject::GetAppID - key.Open failed, keyName : %ws, result : %d", keyName, res);

		return AtlHresultFromWin32(res);
	}

	ULONG chars = 64;
	WCHAR appId[65] = L"";

	res = key.QueryStringValue(L"AppID", appId, &chars);
	if (res != ERROR_SUCCESS)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "ClassObject::GetAppID - key.QueryStringValue failed, result : %d", res);

		return AtlHresultFromWin32(res);
	}

	return IIDFromString(appId, lpGuid);
}

HRESULT ClassObject::CreateInstance(REFIID riid, void** ppv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ClassObject::CreateInstance");

	ObjectLock lock(this);

	CComQIPtr<IClassFactory> spFactory = m_pUnk;
	if (!spFactory)
		return E_FAIL;

	if (m_aptType == APTTYPE_MTA)
	{
		return spFactory->CreateInstance(nullptr, riid, ppv);
	}

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(m_threadId, &spThreadInfo);
	if (hr != S_OK)
		return hr;

	CAutoPtr<_CreateInstanceParam> spParam(new _CreateInstanceParam);
	if (!spParam)
		return E_OUTOFMEMORY;

	spParam->pFactory = spFactory;
	spParam->iid = riid;
	spParam->ppv = ppv;
	spParam->hr = E_PENDING;
	spParam->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!spParam->hEvent)
		return AtlHresultFromLastError();

	PostMessage(spThreadInfo->m_hWnd, RPC_WM_INVOKE, (WPARAM)OnCreateInstance, (LPARAM)spParam.m_p);
	DWORD wait = WaitForSingleObject(spParam->hEvent, INFINITE);

	CloseHandle(spParam->hEvent);

	if (wait == WAIT_OBJECT_0)
	{
		return spParam->hr;
	}

	return E_UNEXPECTED;
}

LRESULT ClassObject::OnCreateInstance(LPARAM lParam)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ClassObject::OnCreateInstance");

	_CreateInstanceParam* pParam = (_CreateInstanceParam*)lParam;
	if (pParam)
	{
		pParam->hr = pParam->pFactory->CreateInstance(nullptr, pParam->iid, pParam->ppv);

		SetEvent(pParam->hEvent);
	}

	return 0;
}

// ClassObjectMap

ClassObjectMap::ClassObjectMap()
{
}

ClassObjectMap::~ClassObjectMap()
{
}

HRESULT ClassObjectMap::Initialize()
{
	return S_OK;
}

void ClassObjectMap::Uninitialize()
{
	m_map.Clear();
}

HRESULT ClassObjectMap::Insert(REFCLSID rclsid, LPUNKNOWN lpUnk, DWORD clsContext, DWORD regFlags, DWORD usage, LPDWORD pCookie)
{
	if (!lpUnk)
		return E_POINTER;

	HRESULT hr = m_map.Lookup(rclsid, IID_NULL, nullptr);
	if (hr == S_OK)
		return S_FALSE;

	CComPtr<ClassObject> spObject;
	hr = CComCreator<CComObject<ClassObject>>::CreateInstance(nullptr, __uuidof(ClassObject), (void**)&spObject);
	if (FAILED(hr))
		return hr;

	hr = spObject->Init(rclsid, clsContext, regFlags, usage, lpUnk);
	if (FAILED(hr))
	{
		return hr;
	}

	GuidMap::_Position pos = nullptr;
	hr = m_map.Insert(rclsid, spObject, &pos);
	if (FAILED(hr))
	{
		return hr;
	}

	if (pCookie)
	{
		*pCookie = (DWORD)pos;
	}

	return S_OK;
}

HRESULT ClassObjectMap::Lookup(REFCLSID rclsid, ClassObject** ppObject)
{
	return m_map.Lookup(rclsid, __uuidof(ClassObject), (void**)ppObject);
}

HRESULT ClassObjectMap::Remove(DWORD cookie)
{
	return m_map.Remove((GuidMap::_Position)cookie);
}

HRESULT ClassObjectMap::Remove(REFCLSID rclsid)
{
	return m_map.Remove(rclsid);
}
