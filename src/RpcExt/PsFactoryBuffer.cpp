#include "StdAfx.h"
#include "PsFactoryBuffer.h"

#include "RpcExt.h"

PsFactoryBuffer::PsFactoryBuffer()
{
	m_ref = 0;

	m_pFactoryBuffer = nullptr;
}

PsFactoryBuffer::~PsFactoryBuffer()
{
	if (m_pFactoryBuffer)
	{
		m_pFactoryBuffer->Release();
	}

	AtlTrace("PsFactoryBuffer::~PsFactoryBuffer\n");
}

STDMETHODIMP PsFactoryBuffer::QueryInterface(REFIID riid, void** ppvObject)
{
	if (InlineIsEqualGUID(riid, IID_IUnknown) || InlineIsEqualGUID(riid, IID_IPSFactoryBuffer))
	{
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) PsFactoryBuffer::AddRef()
{
	return InterlockedIncrement(&m_ref);
}

STDMETHODIMP_(ULONG) PsFactoryBuffer::Release()
{
	ULONG ref = InterlockedDecrement(&m_ref);
	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

STDMETHODIMP PsFactoryBuffer::CreateProxy(IUnknown* pUnkOuter, REFIID riid, IRpcProxyBuffer** ppProxy, void** ppv)
{
	if (!ppProxy || !ppv)
		return E_POINTER;

	//HRESULT hr = _CreateProxy(pUnkOuter, riid, ppProxy, ppv);
	HRESULT hr = m_pFactoryBuffer->CreateProxy(pUnkOuter, riid, ppProxy, ppv);

	return hr;
}

STDMETHODIMP PsFactoryBuffer::CreateStub(REFIID riid, IUnknown* pUnkServer, IRpcStubBuffer** ppStub)
{
	if (!ppStub)
		return E_POINTER;

	CComPtr<IRpcStubBuffer> spStubBuffer;
	//HRESULT hr = _CreateStub(riid, pUnkServer, &spStubBuffer);
	HRESULT hr = m_pFactoryBuffer->CreateStub(riid, pUnkServer, &spStubBuffer);
	if (FAILED(hr))
		return hr;

	if (InlineIsEqualGUID(riid, IID_IDispatch))
		*ppStub = spStubBuffer.Detach();
	else
		hr = RpcStubBuffer::CreateInstance(riid, pUnkServer, spStubBuffer, ppStub);

	return hr;
}

HRESULT PsFactoryBuffer::_CreateProxy(IUnknown* pUnkOuter, REFIID riid, IRpcProxyBuffer** ppProxy, void** ppv)
{
	if (InlineIsEqualGUID(riid, IID_NULL) || InlineIsEqualGUID(riid, IID_IUnknown))
		return E_INVALIDARG;

	CComPtr<IPSFactoryBuffer> spFactoryBuffer;
	HRESULT hr = CreateFactoryBuffer(riid, &spFactoryBuffer);
	if (FAILED(hr))
		return hr;

	hr = spFactoryBuffer->CreateProxy(pUnkOuter, riid, ppProxy, ppv);

	return hr;
}

HRESULT PsFactoryBuffer::_CreateStub(REFIID riid, IUnknown* pUnkServer, IRpcStubBuffer** ppStub)
{
	if (InlineIsEqualGUID(riid, IID_NULL) || InlineIsEqualGUID(riid, IID_IUnknown))
		return E_INVALIDARG;

	CComPtr<IPSFactoryBuffer> spFactoryBuffer;
	HRESULT hr = CreateFactoryBuffer(riid, &spFactoryBuffer);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "PsFactoryBuffer::_CreateStub, CreateFactoryBuffer");
		return hr;
	}

	hr = spFactoryBuffer->CreateStub(riid, pUnkServer, ppStub);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, "PsFactoryBuffer::_CreateStub, CreateStub");
	}

	return hr;
}

typedef HRESULT(__stdcall* _DllGetClassObject)(REFCLSID, REFIID, void**);

HRESULT PsFactoryBuffer::CreateFactoryBuffer(REFIID riid, IPSFactoryBuffer** ppBuffer)
{
	if (!ppBuffer)
		return E_POINTER;

	if (InlineIsEqualGUID(riid, IID_NULL))
		return E_INVALIDARG;

	CComBSTR clsid_str;
	HRESULT hr = GetProxyStubClsid(riid, &clsid_str);
	if (hr != S_OK)
		return hr;

	CLSID clsid = CLSID_NULL;
	hr = IIDFromString(clsid_str, &clsid);
	if (hr != S_OK)
		return hr;

	WCHAR fileName[MAX_PATH] = L"";
	hr = GetProxyDll(clsid_str, fileName, MAX_PATH);
	if (hr != S_OK)
		return hr;

	WCHAR path[MAX_PATH] = L"";
	ExpandEnvironmentStrings(fileName, path, MAX_PATH);

	HINSTANCE hInst = LoadLibrary(path);
	if (!hInst)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "PsFactoryBuffer::CreateFactoryBuffer, LoadLibrary, fileNmae : %s", CW2A(fileName).m_psz);
		return AtlHresultFromLastError();
	}

	auto pfn = (_DllGetClassObject)GetProcAddress(hInst, "DllGetClassObject");
	if (!pfn)
		return AtlHresultFromLastError();

	hr = pfn(clsid, __uuidof(IPSFactoryBuffer), (void**)ppBuffer);

	return hr;
}

HRESULT PsFactoryBuffer::GetProxyStubClsid(REFIID riid, BSTR* pClsid)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "PsFactoryBuffer::GetProxyStubClsid");

	ATLASSERT(pClsid);

	WCHAR keyName[MAX_PATH + 1] = L"Interface\\";
	ULONG n = wcslen(keyName);
	int _ = StringFromGUID2(riid, keyName + n, MAX_PATH - n);
	wcscat_s(keyName, MAX_PATH, L"\\ProxyStubClsid32");

	CRegKey key;
	LONG result = key.Open(HKEY_CLASSES_ROOT, keyName, KEY_READ);
	if (result != ERROR_SUCCESS)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, result, "PsFactoryBuffer::GetProxyStubClsid, key.Open, keyName : %s", CW2A(keyName).m_psz);
		return HRESULT_FROM_WIN32(result);
	}

	ULONG chars = 0;
	result = key.QueryStringValue(nullptr, nullptr, &chars);
	if (result != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(result);

	*pClsid = SysAllocStringLen(nullptr, chars);
	if (*pClsid == nullptr)
		return E_OUTOFMEMORY;

	result = key.QueryStringValue(nullptr, *pClsid, &chars);

	return S_OK;
}

HRESULT PsFactoryBuffer::GetProxyDll(LPCWSTR clsid, LPWSTR fileName, DWORD size)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "PsFactoryBuffer::GetProxyDll");

	ATLASSERT(clsid);
	ATLASSERT(fileName);

	WCHAR keyName[MAX_PATH + 1] = L"";
	swprintf_s(keyName, MAX_PATH, L"CLSID\\%ws\\InProcServer32", clsid);

	CRegKey key;
	LONG result = key.Open(HKEY_CLASSES_ROOT, keyName, KEY_READ);
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"PsFactoryBuffer::GetProxyDll, key.Open, keyName : %ws, result : %d", keyName, result);
	if (result != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(result);
	}

	result = key.QueryStringValue(nullptr, fileName, &size);
	if (result != ERROR_SUCCESS)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "PsFactoryBuffer::GetProxyDll, QueryStringValue failed, result : %d", result);
		return HRESULT_FROM_WIN32(result);
	}

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"PsFactoryBuffer::GetProxyDll - end, fileName : %ws", fileName);

	return S_OK;
}

static WCHAR _clsidKey[] = L"CLSID";
static WCHAR _psClsidKey[] = L"{4343405D-69BD-4b60-A3AA-A5F2741F984A}";

HRESULT PsFactoryBuffer::RegisterServer()
{
	CRegKey keyClsid;
	LONG res = keyClsid.Open(HKEY_CLASSES_ROOT, _clsidKey, KEY_WRITE);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	CRegKey key;
	res = key.Create(keyClsid, _psClsidKey);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	key.SetStringValue(nullptr, L"PSFactoryBuffer");

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

HRESULT PsFactoryBuffer::UnregisterServer()
{
	CRegKey keyClsid;
	LONG res = keyClsid.Open(HKEY_CLASSES_ROOT, _clsidKey, KEY_WRITE);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	res = keyClsid.DeleteSubKey(_psClsidKey);
	if (res != ERROR_SUCCESS)
		return AtlHresultFromWin32(res);

	return S_OK;
}

HRESULT PsFactoryBuffer::GetClassObject(IPSFactoryBuffer* pFactoryBuffer, REFIID riid, void** ppv)
{
	ATLASSERT(ppv);
	ATLASSERT(InlineIsEqualGUID(riid, IID_IPSFactoryBuffer));

	auto p = new PsFactoryBuffer;
	if (!p)
	{
		*ppv = nullptr;
		pFactoryBuffer->Release();
		return E_OUTOFMEMORY;
	}

	p->AddRef();
	p->m_pFactoryBuffer = pFactoryBuffer;

	*ppv = p;

	return S_OK;
}