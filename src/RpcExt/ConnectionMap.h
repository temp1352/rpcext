#pragma once

#include "ClientConnection.h"

class RpcConnectionMap
{
public:
	RpcConnectionMap();
	~RpcConnectionMap();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT Insert(RpcClientConnection* pConnection);
	HRESULT Lookup(REFGUID cid, RpcClientConnection** ppConnection);

	BOOL Remove(RpcClientConnection* pConnection);

	HRESULT InitBins();

	void Clear();
	void _Clear();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_binCount;
	RpcClientConnection** m_ppBins;
};