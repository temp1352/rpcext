#pragma once

class Handles
{
public:
	enum
	{
		WaitTimeout = -1,
		WaitFail = -2,
	};

	Handles();
	~Handles();

	ULONG GetCount() { return m_count; }
	HANDLE operator[](ULONG i) { return m_pHandle[i]; }

	HRESULT Add(HANDLE handle);
	BOOL Remove(HANDLE handle);
	BOOL RemoveAt(ULONG index);
	void RemoveAll();

	void Close();

	int Wait(BOOL bWaitAll, DWORD dwMilliseconds);
	int WaitAll(DWORD dwMilliseconds);

	void PostQueuedCompletionStatus(HANDLE hIOCP, DWORD dwTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped);

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_count;
	ULONG m_allocSize;
	HANDLE* m_pHandle;
};