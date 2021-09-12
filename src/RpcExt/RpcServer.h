#pragma once

#include "Handles.h"
#include "SocketList.h"
#include "ConnectionList.h"
#include "ServerConnection.h"

class RE_API RpcServer
{
public:
	friend RpcServerConnection;

	RpcServer();
	~RpcServer();

	HRESULT Initialize();
	void Uninitialize();

	ULONG GetMaxThreads();
	HRESULT SetMaxThreads(ULONG maxThreads);

	ULONG GetMaxConnections();
	HRESULT SetMaxConnections(ULONG maxConnections);

	HRESULT Startup(LPCWSTR appName);
	HRESULT Startup(LONG port);
	HRESULT Stop();

	DWORD WorkProc();
	static DWORD WINAPI _WorkProc(LPVOID pv);

	DWORD IdleProc();
	static DWORD WINAPI _IdleProc(LPVOID pv);

	HRESULT NewConnection();
	HRESULT GetConnection(RPC_SID sid, RpcConnection** ppConnection);

	void OnClose(RpcServerConnection* pConnection);

protected:
	CComCriticalSection m_cs;

	RE_SERVICE_STATE m_state;

	LONG m_port;

	ListenSocket m_socket;

	ULONG m_maxThreads;
	ULONG m_maxConnections;

	HANDLE m_hIOCP;
	HANDLE m_hStopEvent;

	Handles m_threads;

	ConnectionList m_connList;
	ConnectionList m_acceptList;
};
