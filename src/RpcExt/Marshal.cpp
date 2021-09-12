#include "StdAfx.h"
#include "Marshal.h"

#include "RpcExt.h"
#include "ProxyManager.h"
#include "InterfaceMap.h"
#include "ObjRef.h"

Marshal::Marshal()
{
}

Marshal::~Marshal()
{
	AtlTrace("Marshal::~Marshal\n");
}

STDMETHODIMP Marshal::GetUnmarshalClass(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, CLSID* pCid)
{
	*pCid = __uuidof(MarshalClassFactory);

	return S_OK;
}

STDMETHODIMP Marshal::GetMarshalSizeMax(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, DWORD* pSize)
{
	return 0;
}

STDMETHODIMP Marshal::MarshalInterface(IStream* pStm, REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags)
{
	return 0;
}

STDMETHODIMP Marshal::UnmarshalInterface(IStream* pStm, REFIID riid, void** ppv)
{
	if (_logFile.m_outLevel > 1)
	{
		CStringA strItf;
		_interfaceMap.GetInterfaceName(riid, strItf);
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "Marshal::UnmarshalInterface, riid : %s", strItf);
	}

	ConnectInfo connInfo;
	HRESULT hr = connInfo.Read(pStm);
	if (FAILED(hr))
		return hr;

	CComPtr<ProxyManager> spProxyManager;
	hr = _rpcExt.m_proxyMgrList.GetProxyManager(connInfo.m_cid, connInfo.m_oid, &spProxyManager);
	if (hr != S_OK)
	{
		hr = ProxyManager::_CreateInstance(__uuidof(ProxyManager), (void**)&spProxyManager);
		if (hr != S_OK)
			return hr;

		hr = spProxyManager->Create(riid, &connInfo);
	}

	hr = spProxyManager->GetProxy(riid, ppv);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "Marshal::UnmarshalInterface, hr : %x", hr);

	return hr;
}

STDMETHODIMP Marshal::ReleaseMarshalData(IStream* pStm)
{
	return 0;
}

STDMETHODIMP Marshal::DisconnectObject(DWORD dwReserved)
{
	AtlTrace("Marshal::DisconnectObject\n");

	return 0;
}

// MarshalClassFactory

MarshalClassFactory::MarshalClassFactory()
{
}

STDMETHODIMP MarshalClassFactory::QueryInterface(REFIID riid, void** ppvObject)
{
	if (InlineIsEqualGUID(riid, IID_IUnknown) || InlineIsEqualGUID(riid, IID_IClassFactory))
	{
		*ppvObject = this;
		AddRef();

		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) MarshalClassFactory::AddRef()
{
	_pAtlModule->Lock();

	return 1;
}

STDMETHODIMP_(ULONG) MarshalClassFactory::Release()
{
	_pAtlModule->Unlock();

	return 1;
}

STDMETHODIMP MarshalClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject)
{
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	CComObject<Marshal>* p = nullptr;
	HRESULT hr = p->CreateInstance(&p);
	if (hr == S_OK)
	{
		p->AddRef();
		hr = p->QueryInterface(riid, ppvObject);
		p->Release();
	}

	return hr;
}

STDMETHODIMP MarshalClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		_pAtlModule->Lock();
	else
		_pAtlModule->Unlock();

	return S_OK;
}

WCHAR _clsidKey[] = L"CLSID";
WCHAR _marshalClsidKey[] = L"{BF36F5B0-29E3-4353-8F6A-2688C56500A9}";

HRESULT MarshalClassFactory::RegisterServer()
{
	CRegKey keyClsid;
	LONG res = keyClsid.Open(HKEY_CLASSES_ROOT, _clsidKey, KEY_WRITE);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	CRegKey key;
	res = key.Create(keyClsid, _marshalClsidKey);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	key.SetStringValue(nullptr, L"DelegateMarshal");

	CRegKey keyServer;
	res = keyServer.Create(key, L"InProcServer32");
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	WCHAR fileName[MAX_PATH] = L"";
	::GetModuleFileName(_AtlBaseModule.GetModuleInstance(), fileName, MAX_PATH);
	res = keyServer.SetStringValue(nullptr, fileName);

	res = keyServer.SetStringValue(L"ThreadingModel", L"Both");

	return S_OK;
}

HRESULT MarshalClassFactory::UnregisterServer()
{
	CRegKey keyClsid;
	LONG res = keyClsid.Open(HKEY_CLASSES_ROOT, _clsidKey, KEY_WRITE);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	res = keyClsid.DeleteSubKey(_marshalClsidKey);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	return S_OK;
}

static MarshalClassFactory _marshalFactory;
static DWORD _cookie = 0;

HRESULT MarshalClassFactory::RegisterClassObject()
{
	HRESULT hr = CoRegisterClassObject(__uuidof(MarshalClassFactory), &_marshalFactory, CLSCTX_INPROC_SERVER, REGCLS_MULTIPLEUSE, &_cookie);

	return hr;
}

HRESULT MarshalClassFactory::RevokeClassObject()
{
	HRESULT hr = S_OK;

	if (_cookie)
	{
		hr = CoRevokeClassObject(_cookie);
	}

	return hr;
}

HRESULT MarshalClassFactory::GetClassObject(REFIID riid, void** ppv)
{
	return _marshalFactory.QueryInterface(riid, ppv);
}