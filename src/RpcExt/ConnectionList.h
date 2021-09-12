#pragma once

#include "Connection.h"

class ConnectionList
{
public:
	struct _Node
	{
		RpcConnection* pConn;
		_Node* pNext;
		_Node* pPrev;
		_Node* pLink;
	};

	ConnectionList();
	~ConnectionList();

	HRESULT Initialize(ULONG binCount);
	void Term();

	HRESULT GetConnection(REFGUID cid, RpcConnection** ppConnect);
	HRESULT GetConnection(ULONG_PTR key, bool remove, RpcConnection** ppConnect);

	HRESULT Add(RpcConnection* pConnection);

	BOOL Remove(ULONG_PTR key);
	BOOL Remove(RpcConnection* pConnection);

	void RemoveAll();
	void Remove(_Node* pNode);

	void CloseAll();

	_Node* GetHead() { return m_pHead; }
	_Node* GetTail() { return m_pTail; }

	ULONG GetCount() { return m_count; }

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	ULONG m_count;
	ULONG m_binCount;

	_Node* m_pHead;
	_Node* m_pTail;
	_Node* m_pCache;
	_Node** m_ppBins;

	CComAutoCriticalSection m_cs;
};