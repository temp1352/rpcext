#pragma once

#include "InvokeHandler.h"

class RpcConnection;
class StubManager;
class StubManagerList;
class ClassObject;
class RpcCallContext;

class RpcStub : public CComObjectRootEx<CComMultiThreadModel>,
				public IUnknown
{
public:
	friend InvokeHandler;
	friend StubManager;
	friend StubManagerList;

	RpcStub();
	~RpcStub();

	BEGIN_COM_MAP(RpcStub)
		COM_INTERFACE_ENTRY(IUnknown)
	END_COM_MAP()

	HRESULT Init(StubManager* pStubManager, REFIID riid, IRpcStubBuffer* pBuffer);
	void Close();

protected:
	StubManager* m_pStubManager;

	IID m_iid;
	IRpcStubBuffer* m_pStubBuffer;

	RpcStub* m_pNext;
};

// StubManager

class __declspec(uuid("F47F7AB8-596D-4aa7-853A-22F9D5B89A40"))
	StubManager : public CComObjectRootEx<CComMultiThreadModel>,
				  public IServerSecurity
{
public:
	friend RpcStub;
	friend InvokeHandler;
	friend RpcCallContext;
	friend StubManagerList;

	StubManager();
	~StubManager();

	static HRESULT _CreateInstance(REFIID riid, void** ppv);

	BEGIN_COM_MAP(StubManager)
		COM_INTERFACE_ENTRY(StubManager)
		COM_INTERFACE_ENTRY(IServerSecurity)
		COM_INTERFACE_ENTRY_FUNC_BLIND(0, _QueryInterface)
	END_COM_MAP()

	static HRESULT __stdcall _QueryInterface(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw);

public:
	HRESULT Create(OID oid, IUnknown* pUnk, APTTYPE type, DWORD threadId);

	void Close();

	OID GetOID() { return m_oid; }

	DWORD AddPrivateRef() { return InterlockedIncrement(&m_privateRef); }
	DWORD ReleasePrivateRef() { return InterlockedDecrement(&m_privateRef); }

	HRESULT GetStub(REFIID riid, RpcStub** ppStub);

	HRESULT CreateStub(REFIID riid, IRpcStubBuffer* pStubBuffer);
	HRESULT _CreateStub(REFIID riid, IRpcStubBuffer* pStubBuffer, RpcStub** ppStub);

	HRESULT Invoke(RpcConnection* pConnection, RpcInvokeRequest* pCmd);

	HRESULT RemQueryInterface(REFIID riid);
	HRESULT _RemQueryInterface(REFIID riid, RpcStub** ppStub);

	HRESULT Safe_QueryInterface(REFIID riid, void** ppv);
	static HRESULT Safe_OnQueryInterface(LPARAM lParam);

	ULONG Safe_AddRef();
	static ULONG Safe_OnAddRef(LPARAM lParam);

	ULONG Safe_Release();
	static ULONG Safe_OnRelease(LPARAM lParam);

	STDMETHOD(QueryBlanket)(DWORD* pAuthnSvc, DWORD* pAuthzSvc, OLECHAR** pServerPrincName,
		DWORD* pAuthnLevel, DWORD* pImpLevel, void** pPrivs, DWORD* pCapabilities);
	STDMETHOD(ImpersonateClient)();
	STDMETHOD(RevertToSelf)();
	STDMETHOD_(BOOL, IsImpersonating)();

protected:
	OID m_oid;

	LONG m_privateRef;

	DWORD m_createThreadId;
	APTTYPE m_aptType;

	IUnknown* m_pUnk;

	RpcStub* m_pFirstStub;
};

struct _QueryInterfaceParam
{
	IUnknown* pUnk;
	IID iid;
	void** ppv;
	HANDLE hEvent;
	HRESULT hr;
};

struct _AddRefParam
{
	IUnknown* pUnk;
	HANDLE hEvent;
	ULONG ref;
};