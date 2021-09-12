#pragma once

#include "StubManager.h"
#include "StdObjList.h"

class AppContext;
class RpcConnection;

class RpcCallContext : public IRpcCallContext
{
public:
	RpcCallContext(RpcConnection* pConnection);
	~RpcCallContext();

	struct _StubMgrNode
	{
		StubManager* pStubManager;
		_StubMgrNode* pNext;
	};

	STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT Connect(REFCLSID rclsid, REFIID riid, OID oid, DWORD flags, SC_AUTH_INFO* pInfo, OID* poid, BSTR* pRefreshToken);

	HRESULT Logon(AppContext* pContext, DWORD flags, SC_AUTH_INFO* pInfo, BSTR* pRefreshToken);

	STDMETHOD(Authentication)(HANDLE* phToken);

	HRESULT CreateInstance(ClassObject* pObject, REFIID riid, IUnknown** ppUnk, OID* poid);

	HRESULT GetAccessToken(IScAccessToken** ppToken);

	HRESULT Invoke(RpcInvokeRequest* pCmd);
	HRESULT QueryInterface(RpcQIRequest* pCmd);
	HRESULT Free(RpcFreeRequest* pCmd);

	HRESULT ProcessInterfacePointers(RpcMessage* pMsg, LONG* pItfPtrCount);
	HRESULT CreateStub(IUnknown* pUnkServer, IRpcStubBuffer* pStubBuffer, StdObjRef* pObjRef, LPBYTE pStart);

	HRESULT GetStubManager(OID oid, StubManager** ppStubManager);
	HRESULT Add(StubManager* pStubManager);
	void Remove(StubManager* pStubManager);

	void RemoveAll();

protected:
	bool m_init;

	RpcConnection* m_pConnection;
	IScAccessToken* m_pAccessToken;

	_StubMgrNode* m_pHeadNode;

	CComCriticalSection m_cs;
};
