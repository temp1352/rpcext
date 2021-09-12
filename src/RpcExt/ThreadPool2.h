#pragma once

#include "ThreadHandles.h"

class RpcThreadPool2
{
public:
	RpcThreadPool2();
	~RpcThreadPool2();

	HRESULT Initialize();
	void Uninitialize();

protected:
	HRESULT InitThreads();
	void TermThreads();

	void WorkProc();
	static DWORD WINAPI _WorkProc(LPVOID pv);

protected:
	bool m_init;

	HANDLE m_hStopEvent;
	HANDLE m_hCompletion;

	ThreadHandles m_threads;

	CComSafeDeleteCriticalSection m_cs;
};