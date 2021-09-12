#pragma once

class ThreadHandles
{
public:
	enum
	{
		WaitTimeout = -1,
		WaitFail = -2,
	};

	ThreadHandles();
	~ThreadHandles();

	ULONG GetCount() { return m_count; }

	HRESULT Add(HANDLE handle, DWORD threadId);

	HANDLE Remove(DWORD threadId);

	BOOL RemoveAt(ULONG index);
	void RemoveAll();

	void Close();

	int Wait(BOOL bWaitAll, DWORD milliseconds);
	int WaitAll(DWORD milliseconds);

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_count;
	ULONG m_allocSize;
	HANDLE* m_pHandle;
	DWORD* m_pThreadId;
};