#pragma once

#include "RpcQueue.h"
#include "RpcHandler.h"
#include "ThreadHandles.h"

class RpcTaskList
{
public:
	struct _Node
	{
		IRpcHandler* pHandler;
		_Node* pNext;
	};

	RpcTaskList();
	~RpcTaskList();

	HRESULT AddTail(IRpcHandler* pHandler);
	HRESULT RemoveHead(IRpcHandler** ppHandler);

	void RemoveAll();

	ULONG GetCount();

protected:
	CComAutoCriticalSection m_cs;

	_Node* m_pHead;
	_Node* m_pTail;

	ULONG m_count;
};

// RpcThreadPool

class RpcThreadPool
{
public:
	RpcThreadPool();
	~RpcThreadPool();

	HRESULT Initialize();
	void Uninitialize();

	void WorkProc();
	static DWORD WINAPI _WorkProc(LPVOID pv);

	void ExecuteProc();

	HRESULT Execute(IRpcHandler* pHandler);

	void _ExitThread();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	bool m_init;

	DWORD m_timeout;

	ULONG m_maxThreadCount;
	ULONG m_maxTaskCount;

	volatile LONG m_threadCount;
	volatile LONG m_activeThreadCount;

	CComSafeDeleteCriticalSection m_cs;

	HANDLE m_hStopEvent;
	HANDLE m_hExeEvent;

	ThreadHandles m_threads;

public:
	RpcTaskList m_taskList;
};
