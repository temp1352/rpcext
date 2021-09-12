#include "StdAfx.h"
#include "SocketList.h"

SocketList::SocketList(void)
{
	m_count = 0;

	m_pHead = nullptr;
	m_pTail = nullptr;
}

SocketList::~SocketList(void)
{
	Clear();
}

HRESULT SocketList::AddTail(SOCKET socket)
{
	m_cs.Lock();

	_SocketNode* pNode = new _SocketNode;
	if (!pNode)
	{
		closesocket(socket);
		m_cs.Unlock();
		return E_OUTOFMEMORY;
	}

	pNode->socket = socket;
	pNode->pNext = nullptr;

	if (m_pTail)
		m_pTail->pNext = pNode;
	else
		m_pHead = pNode;

	m_pTail = pNode;

	++m_count;

	m_cs.Unlock();

	return S_OK;
}

SOCKET SocketList::RemoveHead()
{
	SOCKET socket = NULL;

	m_cs.Lock();

	HRESULT hr = S_FALSE;

	_SocketNode* pNode = m_pHead;
	if (pNode)
	{
		m_pHead = pNode->pNext;
		if (m_pTail == pNode)
			m_pTail = nullptr;

		--m_count;

		socket = pNode->socket;

		delete pNode;
	}

	m_cs.Unlock();

	return socket;
}

ULONG SocketList::GetCount()
{
	m_cs.Lock();

	ULONG count = m_count;

	m_cs.Unlock();

	return count;
}

void SocketList::Clear()
{
	m_cs.Lock();

	_SocketNode* pNode = m_pHead;
	while (pNode)
	{
		_SocketNode* pTemp = pNode;

		pNode = pNode->pNext;

		closesocket(pTemp->socket);

		delete pTemp;
	}

	m_pHead = nullptr;
	m_pTail = nullptr;
	m_count = 0;

	m_cs.Unlock();
}
