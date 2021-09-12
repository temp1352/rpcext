#pragma once

#include "Connection.h"

#define SEND_OVERLAPPED (LPOVERLAPPED)0
#define CLOSE_OVERLAPPED (LPOVERLAPPED)1

class RpcServer;

class RpcServerConnection : public RpcConnection
{
public:
	friend RpcServer;

	RpcServerConnection();
	~RpcServerConnection();

	static HRESULT _CreateInstance(RpcServerConnection** ppConnection);

	ULONG InternalAddRef();
	ULONG InternalRelease();

	BEGIN_COM_MAP(RpcServerConnection)
		COM_INTERFACE_ENTRY(RpcConnection)
	END_COM_MAP()

	HRESULT Reset();
	HRESULT Create(RpcServer* pServer);
	HRESULT Accept(DWORD receiveDataLength);

	virtual bool OnIdle();

	virtual HRESULT QueueSend(RpcCommand* pCmd);

	virtual void Close();
	virtual void Term();

protected:
	HANDLE m_hIOCP;

	RpcServer* m_pServer;
};
