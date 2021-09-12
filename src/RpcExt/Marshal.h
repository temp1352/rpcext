#pragma once

class Marshal : public CComObjectRootEx<CComSingleThreadModel>,
				public IMarshal
{
public:
	Marshal();
	~Marshal();

	BEGIN_COM_MAP(Marshal)
		COM_INTERFACE_ENTRY(IMarshal)
	END_COM_MAP()

	STDMETHOD(GetUnmarshalClass)(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, CLSID* pCid);
	STDMETHOD(GetMarshalSizeMax)(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, DWORD* pSize);

	STDMETHOD(MarshalInterface)(IStream* pStm, REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags);
	STDMETHOD(UnmarshalInterface)(IStream* pStm, REFIID riid, void** ppv);

	STDMETHOD(ReleaseMarshalData)(IStream* pStm);

	STDMETHOD(DisconnectObject)(DWORD dwReserved);
};

class __declspec(uuid("BF36F5B0-29E3-4353-8F6A-2688C56500A9"))
	MarshalClassFactory : public CComObjectRootEx<CComSingleThreadModel>,
						  public IClassFactory
{
public:
	MarshalClassFactory(void);

	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	STDMETHOD(CreateInstance)(IUnknown* pUnkOuter, REFIID riid, void** ppvObject);
	STDMETHOD(LockServer)(BOOL fLock);

	static HRESULT RegisterServer();
	static HRESULT UnregisterServer();

	static HRESULT RegisterClassObject();
	static HRESULT RevokeClassObject();

	static HRESULT GetClassObject(REFIID riid, void** ppv);
};