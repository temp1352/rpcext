#pragma once

#include "Command.h"

class RpcQueue
{
public:
	struct _Node
	{
		RpcCommand* pCmd;
		_Node* pNext;
		_Node* pPrev;
	};

	RpcQueue();
	~RpcQueue();

	HRESULT AddHead(RpcCommand* pCmd);
	HRESULT AddTail(RpcCommand* pCmd);

	HRESULT GetHead(RpcCommand** ppCmd);
	HRESULT GetTail(RpcCommand** ppCmd);

	HRESULT Lookup(RpcCommandType type, RPC_COOKIE cookie, RpcCommand** ppCmd, BOOL bRemove);

	void Remove(RpcCommand* pCmd);

	HRESULT RemoveHead(RpcCommand** ppCmd);
	HRESULT RemoveTail(RpcCommand** ppCmd);

	void RemoveAll();

	ULONG GetCount();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

	//以下几个方法不能并发访问

	ULONG _GetCount() { return m_count; }

	_Node* _GetHead() { return m_pHead; }
	_Node* _GetTail() { return m_pTail; };

	void _Remove(_Node* pNode);
	void _RemoveAll();

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_count;

	_Node* m_pHead;
	_Node* m_pTail;
	_Node* m_pCache;
};
