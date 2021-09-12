#pragma once

#include "RpcDef.h"

class EventManager
{
public:
	struct _Event
	{
		RPC_COOKIE cookie;
		HANDLE hEvent;
		BOOL bLock;
	};

	EventManager();
	~EventManager();

	HRESULT Initialize();
	void Uninitialize();

	void SetEvent(int index, RPC_COOKIE cookie);
	int GetEvent(RPC_COOKIE cookie);
	void ReleaseEvent(int index);

	HANDLE GetHandle(int index);

	void Clear();

protected:
	CComAutoCriticalSection m_cs;

	CAtlArray<_Event> m_aEvent;
};

class RpcAutoEvent
{
public:
	RpcAutoEvent(RPC_COOKIE cookie);
	~RpcAutoEvent();

	int m_eventId;
};
