#pragma once

class __declspec(uuid("4343405D-69BD-4b60-A3AA-A5F2741F984A"))
	PsFactoryBuffer : public IPSFactoryBuffer
{
public:
	PsFactoryBuffer();
	~PsFactoryBuffer();

	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	STDMETHOD(CreateProxy)(IUnknown* pUnkOuter, REFIID riid, IRpcProxyBuffer** ppProxy, void** ppv);
	STDMETHOD(CreateStub)(REFIID riid, IUnknown* pUnkServer, IRpcStubBuffer** ppStub);

	static HRESULT _CreateProxy(IUnknown* pUnkOuter, REFIID riid, IRpcProxyBuffer** ppProxy, void** ppv);
	static HRESULT _CreateStub(REFIID riid, IUnknown* pUnkServer, IRpcStubBuffer** ppStub);

	static HRESULT CreateFactoryBuffer(REFIID riid, IPSFactoryBuffer** ppBuffer);
	static HRESULT GetProxyStubClsid(REFIID riid, BSTR* pClsid);
	static HRESULT GetProxyDll(LPCWSTR pszClsid, LPWSTR pszFileName, DWORD cchFileName);

	static HRESULT RegisterServer();
	static HRESULT UnregisterServer();

	static HRESULT GetClassObject(IPSFactoryBuffer* pFactoryBuffer, REFIID riid, void** ppv);

private:
	ULONG m_ref;

	IPSFactoryBuffer* m_pFactoryBuffer;
};
