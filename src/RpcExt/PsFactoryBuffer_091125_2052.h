#pragma once

class __declspec(uuid("4343405D-69BD-4b60-A3AA-A5F2741F984A"))
	CPsFactoryBuffer : public IPSFactoryBuffer
{
public:
	CPsFactoryBuffer();
	~CPsFactoryBuffer();

	HRESULT Initialize();
	void Uninitialize();

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void** ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef(void);

	virtual ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT STDMETHODCALLTYPE CreateProxy(
		/* [in] */ IUnknown* pUnkOuter,
		/* [in] */ REFIID riid,
		/* [out] */ IRpcProxyBuffer** ppProxy,
		/* [out] */ void** ppv);

	virtual HRESULT STDMETHODCALLTYPE CreateStub(
		/* [in] */ REFIID riid,
		/* [unique][in] */ IUnknown* pUnkServer,
		/* [out] */ IRpcStubBuffer** ppStub);

	static HRESULT CreateFactoryBuffer(REFIID riid, IPSFactoryBuffer** ppBuffer);
	static HRESULT GetProxyStubClsid(REFIID riid, BSTR* pClsid);
	static HRESULT GetProxyDll(LPCWSTR pszClsid, LPWSTR pszFileName, DWORD cchFileName);

	static HRESULT RegisterServer();
	static HRESULT UnregisterServer();
};

extern CPsFactoryBuffer _factoryBuffer;