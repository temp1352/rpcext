#include "stdafx.h"
#include "Handles.h"

Handles::Handles()
{
	m_count = 0;
	m_allocSize = 0;
	m_pHandle = nullptr;
}

Handles::~Handles()
{
	RemoveAll();
}

HRESULT Handles::Add(HANDLE handle)
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
		m_allocSize = nAlloc;
	}

	m_pHandle[m_count] = handle;
	++m_count;

	m_cs.Unlock();

	return S_OK;
}

BOOL Handles::Remove(HANDLE handle)
{
	BOOL bRet = FALSE;

	m_cs.Lock();

	for (int i = 0; i < m_count; ++i)
	{
		if (m_pHandle[i] == handle)
		{
			HANDLE* pHandle = m_pHandle + i;
			memmove(pHandle, pHandle + 1, (m_count - i - 1) * sizeof(HANDLE));

			--m_count;

			bRet = TRUE;
			break;
		}
	}

	m_cs.Unlock();

	return bRet;
}

BOOL Handles::RemoveAt(ULONG index)
{
	m_cs.Lock();

	if (index >= m_count)
	{
		m_cs.Unlock();
		return FALSE;
	}

	HANDLE* pHandle = m_pHandle + index;
	memmove(pHandle, pHandle + 1, (m_count - index - 1) * sizeof(HANDLE));

	--m_count;

	m_cs.Unlock();

	return TRUE;
}

void Handles::RemoveAll()
{
	m_cs.Lock();

	if (m_pHandle)
	{
		free(m_pHandle);
		m_pHandle = nullptr;
	}

	m_count = 0;
	m_allocSize = 0;

	m_cs.Unlock();
}

void Handles::Close()
{
	m_cs.Lock();

	for (int i = 0; i < m_count; ++i)
	{
		CloseHandle(m_pHandle[i]);
	}

	m_cs.Unlock();
}

int Handles::Wait(BOOL bWaitAll, DWORD dwMilliseconds)
{
	m_cs.Lock();

	int ret = WaitFail;

	DWORD dwWait = WaitForMultipleObjects(m_count, m_pHandle, bWaitAll, dwMilliseconds);

	if (dwWait >= WAIT_OBJECT_0 && dwWait <= WAIT_OBJECT_0 + m_count)
		ret = dwWait - WAIT_OBJECT_0;
	else if (dwWait == WAIT_TIMEOUT)
		ret = WaitTimeout;

	m_cs.Unlock();

	return ret;
}

int Handles::WaitAll(DWORD dwMilliseconds)
{
	m_cs.Lock();

	int ret = WaitFail;

	int nRemain = m_count;
	while (nRemain)
	{
		HANDLE* pHandle = m_pHandle;
		int n = min(nRemain, MAXIMUM_WAIT_OBJECTS);
		DWORD dwWait = WaitForMultipleObjects(n, pHandle, TRUE, dwMilliseconds);
		if (dwWait == WAIT_TIMEOUT)
		{
			ret = WaitTimeout;
			break;
		}

		nRemain -= n;
		pHandle += n;
	}

	m_cs.Unlock();

	return ret;
}

void Handles::PostQueuedCompletionStatus(HANDLE hIOCP, DWORD dwTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped)
{
	m_cs.Lock();

	for (int i = 0; i < m_count; ++i)
	{
		::PostQueuedCompletionStatus(hIOCP, dwTransferred, dwCompletionKey, lpOverlapped);
	}

	m_cs.Unlock();
}