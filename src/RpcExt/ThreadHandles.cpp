#include "StdAfx.h"
#include "ThreadHandles.h"

ThreadHandles::ThreadHandles()
{
	m_count = 0;
	m_allocSize = 0;
	m_pHandle = nullptr;
	m_pThreadId = nullptr;
}

ThreadHandles::~ThreadHandles()
{
	RemoveAll();
}

HRESULT ThreadHandles::Add(HANDLE handle, DWORD threadId)
{
	m_cs.Lock();

	if (m_count >= m_allocSize)
	{
		ULONG nAlloc = m_allocSize * 2 + 1;
		HANDLE* pHandle = (HANDLE*)realloc(m_pHandle, nAlloc * sizeof(HANDLE));
		if (!pHandle)
		{
			m_cs.Unlock();
			return E_OUTOFMEMORY;
		}

		m_pHandle = pHandle;

		DWORD* pThreadId = (DWORD*)realloc(m_pThreadId, nAlloc * sizeof(DWORD));
		if (!pThreadId)
		{
			m_cs.Unlock();
			return E_OUTOFMEMORY;
		}

		m_pThreadId = pThreadId;

		m_allocSize = nAlloc;
	}

	m_pHandle[m_count] = handle;
	m_pThreadId[m_count] = threadId;

	++m_count;

	m_cs.Unlock();

	return S_OK;
}

HANDLE ThreadHandles::Remove(DWORD dwTheadId)
{
	HANDLE hThread = nullptr;

	m_cs.Lock();

	for (int i = 0; i < m_count; ++i)
	{
		if (m_pThreadId[i] == dwTheadId)
		{
			hThread = m_pHandle[i];

			HANDLE* pHandle = m_pHandle + i;
			memmove(pHandle, pHandle + 1, (m_count - i - 1) * sizeof(HANDLE));

			DWORD* pThreadId = m_pThreadId + i;
			memmove(pThreadId, pThreadId + 1, (m_count - i - 1) * sizeof(DWORD));

			--m_count;

			break;
		}
	}

	m_cs.Unlock();

	return hThread;
}

BOOL ThreadHandles::RemoveAt(ULONG index)
{
	m_cs.Lock();

	if (index >= m_count)
	{
		m_cs.Unlock();
		return FALSE;
	}

	HANDLE* pHandle = m_pHandle + index;
	memmove(pHandle, pHandle + 1, (m_count - index - 1) * sizeof(HANDLE));

	DWORD* pThreadId = m_pThreadId + index;
	memmove(pThreadId, pThreadId + 1, (m_count - index - 1) * sizeof(DWORD));

	--m_count;

	m_cs.Unlock();

	return TRUE;
}

void ThreadHandles::RemoveAll()
{
	m_cs.Lock();

	if (m_pHandle)
	{
		free(m_pHandle);
		m_pHandle = nullptr;
	}

	if (m_pThreadId)
	{
		free(m_pThreadId);
		m_pThreadId = nullptr;
	}

	m_count = 0;
	m_allocSize = 0;

	m_cs.Unlock();
}

void ThreadHandles::Close()
{
	m_cs.Lock();

	for (int i = 0; i < m_count; ++i)
	{
		CloseHandle(m_pHandle[i]);
	}

	m_cs.Unlock();
}

int ThreadHandles::Wait(BOOL bWaitAll, DWORD milliseconds)
{
	m_cs.Lock();

	int ret = WaitFail;

	DWORD dwWait = WaitForMultipleObjects(m_count, m_pHandle, bWaitAll, milliseconds);

	if (dwWait >= WAIT_OBJECT_0 && dwWait <= WAIT_OBJECT_0 + m_count)
		ret = dwWait - WAIT_OBJECT_0;
	else if (dwWait == WAIT_TIMEOUT)
		ret = WaitTimeout;

	m_cs.Unlock();

	return ret;
}

int ThreadHandles::WaitAll(DWORD milliseconds)
{
	m_cs.Lock();

	int ret = WaitFail;

	HANDLE* pHandle = m_pHandle;
	int remain = m_count;
	while (remain)
	{
		int n = min(remain, MAXIMUM_WAIT_OBJECTS);
		DWORD dwWait = WaitForMultipleObjects(n, pHandle, TRUE, milliseconds);
		if (dwWait == WAIT_TIMEOUT)
		{
			ret = WaitTimeout;
			break;
		}

		remain -= n;
		pHandle += n;
	}

	m_cs.Unlock();

	return ret;
}