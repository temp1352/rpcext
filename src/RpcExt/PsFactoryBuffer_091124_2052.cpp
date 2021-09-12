#include "StdAfx.h"
#include "PsFactoryBuffer.h"

#include "RpcExt.h"

#include "Dbghelp.h"

CPsFactoryBuffer::CPsFactoryBuffer()
{
}

CPsFactoryBuffer::~CPsFactoryBuffer()
{
}

BOOL ReplaceIATEntry(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew, HMODULE hModCaller)
{
	ULONG ulSize = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
		hModCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	if (pImportDesc == NULL)
		return FALSE;

	for (; pImportDesc->Name; ++pImportDesc)
	{
		LPCSTR pszModName = (LPCSTR)((LPBYTE)hModCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, pszCalleeModName) == 0)
			break;
	}

	if (pImportDesc->Name == 0)
		return FALSE;

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((LPBYTE)hModCaller + pImportDesc->FirstThunk);
	for (; pThunk->u1.Function; ++pThunk)
	{
		PROC* ppfn = (PROC*)&pThunk->u1.Function;
		BOOL fFound = (*ppfn == pfnCurrent);
		if (fFound)
		{
			WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
			return TRUE;
		}
	}

	return FALSE;
}

HRESULT CPsFactoryBuffer::Initialize()
{

	return S_OK;
}

void CPsFactoryBuffer::Uninitialize()
{
}

HRESULT STDMETHODCALLTYPE CPsFactoryBuffer::QueryInterface(
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void** ppvObject)
{
	if (InlineIsEqualGUID(riid, IID_IUnknown) || InlineIsEqualGUID(riid, IID_IPSFactoryBuffer))
	{
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CPsFactoryBuffer::AddRef(void)
{
	return 1;
}

ULONG STDMETHODCALLTYPE CPsFactoryBuffer::Release(void)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CPsFactoryBuffer::CreateProxy(
	/* [in] */ IUnknown* pUnkOuter,
	/* [in] */ REFIID riid,
	/* [out] */ IRpcProxyBuffer** ppProxy,
	/* [out] */ void** ppv)
{
	CComPtr<IPSFactoryBuffer> spFactoryBuffer;
	HRESULT hr = CreateFactoryBuffer(riid, &spFactoryBuffer);
	if (FAILED(hr))
		return hr;

	hr = spFactoryBuffer->CreateProxy(pUnkOuter, riid, ppProxy, ppv);
	if (FAILED(hr))
		return hr;

	CComPtr<CProxyManager> spManager;
	hr = _rpcExt.m_proxyMgrList.GetProxyManager(pUnkOuter, &spManager);
	if (hr == S_OK)
	{
		hr = spManager->CreateChannelBuffer(*ppProxy, riid, FALSE);
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE CPsFactoryBuffer::CreateStub(
	/* [in] */ REFIID riid,
	/* [unique][in] */ IUnknown* pUnkServer,
	/* [out] */ IRpcStubBuffer** ppStub)
{
	CComPtr<IPSFactoryBuffer> spFactoryBuffer;
	HRESULT hr = CreateFactoryBuffer(riid, &spFactoryBuffer);
	if (FAILED(hr))
		return hr;

	hr = spFactoryBuffer->CreateStub(riid, pUnkServer, ppStub);
	if (FAILED(hr))
		return hr;

	CComPtr<CStubManager> spManager;
	hr = _rpcExt.m_stubMgrList.GetStubManager(pUnkServer, &spManager);
	if (hr == S_OK)
	{
		hr = spManager->_CreateStub(*ppStub, riid);
	}

	return hr;
}

typedef HRESULT(__stdcall* _DllGetClassObject)(REFCLSID, REFIID, void**);

HRESULT CPsFactoryBuffer::CreateFactoryBuffer(REFIID riid, IPSFactoryBuffer** ppBuffer)
{
	if (!ppBuffer)
		return E_POINTER;

	if (InlineIsEqualGUID(riid, IID_NULL))
		return E_INVALIDARG;

	CComBSTR strClsid;
	HRESULT hr = GetProxyStubClsid(riid, &strClsid);
	if (hr != S_OK)
		return hr;

	CLSID clsid = CLSID_NULL;
	hr = IIDFromString(strClsid, &clsid);
	if (hr != S_OK)
		return hr;

	WCHAR szFileName[MAX_PATH] = L"";
	hr = GetProxyDll(strClsid, szFileName, MAX_PATH);
	if (hr != S_OK)
		return hr;

	HINSTANCE hInst = LoadLibrary(szFileName);
	if (!hInst)
		return AtlHresultFromLastError();

	_DllGetClassObject pfn = (_DllGetClassObject)GetProcAddress(hInst, "DllGetClassObject");
	if (!pfn)
		return AtlHresultFromLastError();

	hr = pfn(clsid, __uuidof(IPSFactoryBuffer), (void**)ppBuffer);

	return hr;
}

HRESULT CPsFactoryBuffer::GetProxyStubClsid(REFIID riid, BSTR* pClsid)
{
	ATLASSERT(pClsid);

	WCHAR szKeyName[260] = L"\\Interface\\";
	StringFromGUID2(riid, szKeyName + wcslen(szKeyName), 260);

	wcscat_s(szKeyName, 260, L"\\_ProxyStubClsid32");

	CRegKey key;
	LONG result = key.Open(HKEY_CLASSES_ROOT, szKeyName, KEY_READ);
	if (result != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(result);

	ULONG nChars = 0;
	result = key.QueryStringValue(NULL, NULL, &nChars);
	if (result != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(result);

	*pClsid = SysAllocStringLen(NULL, nChars);
	if (*pClsid == NULL)
		return E_OUTOFMEMORY;

	result = key.QueryStringValue(NULL, *pClsid, &nChars);

	return S_OK;
}

HRESULT CPsFactoryBuffer::GetProxyDll(LPCWSTR pszClsid, LPWSTR pszFileName, DWORD cchFileName)
{
	ATLASSERT(pszClsid);
	ATLASSERT(pszFileName);

	WCHAR szKeyName[260] = L"";
	swprintf_s(szKeyName, 260, L"\\CLSID\\%ws\\InProcServer32", pszClsid);

	CRegKey key;
	LONG result = key.Open(HKEY_CLASSES_ROOT, szKeyName, KEY_READ);
	if (result != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(result);

	result = key.QueryStringValue(NULL, pszFileName, &cchFileName);
	if (result != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(result);

	return S_OK;
}

HRESULT CPsFactoryBuffer::RegisterServer()
{
	WCHAR _szClsid[260] = L"{4343405D-69BD-4b60-A3AA-A5F2741F984A}";

	CRegKey keyClsid;
	LONG lRes = keyClsid.Open(HKEY_CLASSES_ROOT, _T("CLSID"), KEY_WRITE);
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);

	CRegKey key;
	lRes = key.Create(keyClsid, _szClsid);
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);

	key.SetStringValue(NULL, L"PSFactoryBuffer");

	CRegKey keyServer;
	lRes = keyServer.Create(key, L"InProcServer32");
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);

	WCHAR szFileName[MAX_PATH] = L"";
	::GetModuleFileName(_AtlBaseModule.GetModuleInstance(), szFileName, MAX_PATH);
	lRes = keyServer.SetStringValue(NULL, szFileName);

	lRes = keyServer.SetStringValue(L"ThreadingModel", L"Both");

	return S_OK;
}

HRESULT CPsFactoryBuffer::UnregisterServer()
{
	return S_OK;
}