#pragma once

#include "Connection.h"
#include "ErrorInfo.h"

class RpcConnectionMap;

class RpcClientConnection : public RpcConnection, public IRpcClientConnection
{
	friend RpcConnectionMap;
public:
	RpcClientConnection();
	~RpcClientConnection();

	static HRESULT _CreateInstance(IRpcConnectionSink* pSink, RpcClientConnection** ppConnection);

#ifdef _DEBUG
	ULONG InternalAddRef();
	ULONG InternalRelease();
#endif

	BEGIN_COM_MAP(RpcClientConnection)
		COM_INTERFACE_ENTRY(IRpcClientConnection)
		COM_INTERFACE_ENTRY_CHAIN(RpcConnection)
	END_COM_MAP()

	HRESULT Create(REFCLSID rclsid, REFIID riid, const RE_CONNECTINFO* pConnInfo);
	HRESULT Create(OID oid, REFIID riid, const RE_CONNECTINFO* pConnInfo);

	HRESULT Initialize();
	void Close();

	void ThreadProc();
	void MainProc();
	void ConnectProc();

	HRESULT Connect(RpcBindingRequest* pCmd, BOOL* pCompleted);
	HRESULT Connect(RpcBuffer* pBuffer);

	STDMETHOD_(BOOL, IsConnected)() { return (m_flags & RPC_CF_CONNECTED) != 0; }
	STDMETHOD(HealthCheck)();
	STDMETHOD(Advise)(IRpcConnectionSink* pSink, BOOL addRef);
	STDMETHOD(Unadvise)(IRpcConnectionSink* pSink);

	STDMETHOD_(ULONG, GetConnectCount)();
	STDMETHOD(GetLastConnectResult)();

	virtual HRESULT SendReceive(RpcInvokeRequest* pInvoke, RpcInvokeResp** ppResp);
	virtual HRESULT QueueSend(RpcCommand* pCmd);

	void OnConnected();
	void OnDisconnected();

	virtual bool OnIdle();

	struct _SinkNode
	{
		BOOL addRef;
		IRpcConnectionSink* pSink;
		_SinkNode* pNext;
	};

protected:
	HANDLE m_hThread;
	HANDLE m_hConnEvent;
	HANDLE m_hConnCompEvent;
	HANDLE m_hSendEvent;

	_SinkNode* m_pHeadSink;

	CLSID m_clsid;
	OID m_oid;
	IID m_iid;

	DWORD m_options;
	DWORD m_timeout;
	DWORD m_expireSpan;

	DWORD m_port;
	DWORD m_connectCount;

	CComBSTR m_serverName;
	CComBSTR m_domain;
	CComBSTR m_referer;
	CComBSTR m_params;

	CComBSTR m_sessionId;

	CComBSTR m_authCode;
	CComBSTR m_refreshToken;

	CComBSTR m_userName;
	CComBSTR m_password;

	HRESULT m_hConnectRes;
	ErrorInfoBuffer m_errorInfo;

	LONG m_requesting;
	ULONGLONG m_expireTime;

	ULONG m_hash;
	RpcClientConnection* m_pNext;
};
