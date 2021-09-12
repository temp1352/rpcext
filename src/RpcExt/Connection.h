#pragma once

#include "RpcShell.h"
#include "Socket.h"
#include "RpcQueue.h"
#include "RpcBuffer.h"
#include "CallContext.h"

enum RPC_CONNECTION_FLAG
{
	RPC_CF_CONNECTED = 0x01,
	RPC_CF_SYNC_MODE = 0x10,
	RPC_CF_COMPLETION_ROUTINE = 0x0100,
};

struct RpcRecvBox
{
	RpcBuffer buffer;
	RpcQueue queue;
	WSAOVERLAPPED overlapped;

	RpcRecvBox()
	{
		memset(&overlapped, 0, sizeof(WSAOVERLAPPED));
	}
};

struct RpcSendBox
{
	int test1;
	int test2;
	int failedCount;
	RpcBuffer buffer;
	RpcQueue queue;
	WSAOVERLAPPED overlapped;

	RpcSendBox()
	{
		test1 = test2 = 0;
		failedCount = 0;
		memset(&overlapped, 0, sizeof(WSAOVERLAPPED));
	}
};

class RpcServer;
class InvokeHandler;
class ConnectionList;

class __declspec(uuid("5E3755F3-BA07-45ba-8E09-FC0523473540"))
	RpcConnection : public CComObjectRootEx<CComMultiThreadModel>,
					public IRpcConnection
{
	friend RpcServer;
	friend InvokeHandler;
	friend ConnectionList;
	friend RpcCallContext;

public:
	RpcConnection();
	virtual ~RpcConnection();

	BEGIN_COM_MAP(RpcConnection)
		COM_INTERFACE_ENTRY(RpcConnection)
		COM_INTERFACE_ENTRY(IRpcConnection)
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	ULONG InternalAddRef();
	ULONG InternalRelease();
	ULONG _Release();

	DWORD AddPrivateRef(LONG ref = 1);
	DWORD ReleasePrivateRef(LONG ref = 1);

	virtual HRESULT Initialize();
	virtual void Close();
	virtual void Term();

	STDMETHOD_(BOOL, IsConnected)() { return (m_flags & RPC_CF_CONNECTED) != 0; }
	STDMETHOD(HealthCheck)();
	STDMETHOD(Advise)(IRpcConnectionSink* pSink, BOOL addRef) { return S_OK; }
	STDMETHOD(Unadvise)(IRpcConnectionSink* pSink) { return S_OK; }

	void OnReceiveCompletion(RpcCommand* pCmd);

	void UpdateNextTestTime(ULONGLONG currentTime = 0);

	void CleanSendQueue(ULONGLONG currentTime);
	void CleanRecvQueue(ULONGLONG currentTime);
	void ActiveTest();

	HRESULT Connect();
	HRESULT Disconnect();

	HRESULT InitAuth();
	HRESULT AcceptAuth(HANDLE* phToken);

	HRESULT AuthRequest();

	virtual HRESULT SendReceive(RpcInvokeRequest* pInvoke, RpcInvokeResp** ppResp);

	HRESULT GetInvokeState(RPC_COOKIE invokeCookie, HANDLE hInvokeEvent, RpcInvokeResp** ppResp);
	HRESULT RemQueryInterface(OID oid, REFIID riid);
	HRESULT FreeObject(OID oid);

	DWORD CreateCookie();
	RPC_SID& GetSID() { return m_sid; }

	virtual HRESULT QueueSend(RpcCommand* pCmd);

	void SyncMode();
	void AsyncMode();

protected:
	HRESULT Write(RpcCommand* pCmd);
	HRESULT Read(RpcCommand** ppCmd);

	HRESULT SendNext(bool lock = true);
	HRESULT PostRecv(bool lock = true);

	void OnSended(DWORD error, DWORD transferred);
	void OnReceived(DWORD error, DWORD transferred);

	static void CALLBACK SendCompletion(DWORD error, DWORD transferred, LPWSAOVERLAPPED pOverlapped, DWORD flags);
	static void CALLBACK RecvCompletion(DWORD error, DWORD transferred, LPWSAOVERLAPPED pOverlapped, DWORD flags);

	virtual void OnConnected();
	virtual void OnDisconnected();

	virtual bool OnIdle();

	virtual void OnBinding(RpcBindingRequest* pCmd);
	virtual void OnBindingResp(RpcBindingResp* pCmd);

	virtual void OnAuthenticate(RpcAuthRequest* pCmd);
	virtual void OnAuthenticateResp(RpcAuthResp* pCmd);

	virtual void OnInvoke(RpcInvokeRequest* pCmd);
	virtual void OnInvokeResp(RpcInvokeResp* pCmd);

	virtual void OnQueryState(RpcStateRequest* pCmd);
	virtual void OnQueryStateResp(RpcStateResp* pCmd);

	virtual void OnQueryInterface(RpcQIRequest* pCmd);
	virtual void OnQueryInterfaceResp(RpcQIResp* pCmd);

	virtual void OnActiveTest(RpcActiveRequest* pCmd);
	virtual void OnActiveTestResp(RpcActiveResp* pCmd);

	virtual void OnFree(RpcFreeRequest* pCmd);
	virtual void OnFreeResp(RpcFreeResp* pCmd);

protected:
	DWORD m_flags;

	// auto increment id, 用作ConnectionList的健值，GetQueuedCompletionStatus返回的pKey与Connection关联。如果直接用Connection的指针做键值，
	//当并发量大时，前一个Connection析构，新建的Connection的指针值有可能和前一个相同，当CLOSE_OVERLAPPED状态激活时，
	//pKey会指向新Connection，从而将新Connecton关闭。改用m_aid做健值，每一个Connection初始化时，m_aid递增，在相当长一
	//时间内,m_aid不会重复。
	ULONG_PTR m_aid;

	LONG m_privateRef;

	ULONG m_testCount;
	ULONGLONG m_nextTestTime;

	HANDLE m_hTermEvent;

	GUID m_cid;
	RPC_SID m_sid;

public:
	Socket m_socket;

	RpcCallContext m_callContext;

protected:
	RpcSendBox
		m_sendBox;
	RpcRecvBox m_recvBox;
};
