#include "StdAfx.h"
#include "RpcQueue.h"

RpcQueue::RpcQueue()
{
	m_count = 0;

	m_pHead = nullptr;
	m_pTail = nullptr;
	m_pCache = nullptr;
}

RpcQueue::~RpcQueue()
{
	_RemoveAll();
}

HRESULT RpcQueue::AddHead(RpcCommand* pCmd)
{
	ATLASSERT(pCmd);

	Lock();

	_Node* pNode = m_pCache;
	if (pNode)
	{
		m_pCache = pNode->pNext;
	}
	else
	{
		pNode = new _Node;
		if (!pNode)
		{
			m_cs.Unlock();

			return E_OUTOFMEMORY;
		}
	}

	pCmd->AddRef();
	pNode->pCmd = pCmd;

	pNode->pPrev = nullptr;
	pNode->pNext = m_pHead;

	if (m_pHead)
		m_pHead->pPrev = pNode;
	else
		m_pTail = pNode;

	m_pHead = pNode;

	++m_count;

	Unlock();

	return S_OK;
}

HRESULT RpcQueue::AddTail(RpcCommand* pCmd)
{
	ATLASSERT(pCmd);

	Lock();

	_Node* pNode = m_pCache;
	if (pNode)
	{
		m_pCache = pNode->pNext;
	}
	else
	{
		pNode = new _Node;
		if (!pNode)
		{
			m_cs.Unlock();

			return E_OUTOFMEMORY;
		}
	}

	pCmd->AddRef();
	pNode->pCmd = pCmd;

	pNode->pPrev = m_pTail;
	pNode->pNext = nullptr;

	if (m_pTail)
		m_pTail->pNext = pNode;
	else
		m_pHead = pNode;

	m_pTail = pNode;

	++m_count;

	Unlock();

	return S_OK;
}

HRESULT RpcQueue::GetHead(RpcCommand** ppCmd)
{
	ATLASSERT(ppCmd);

	Lock();

	HRESULT hr = S_OK;

	if (m_pHead)
	{
		*ppCmd = m_pHead->pCmd;
		(*ppCmd)->AddRef();
	}
	else
	{
		hr = S_FALSE;
	}

	Unlock();

	return hr;
}

HRESULT RpcQueue::GetTail(RpcCommand** ppCmd)
{
	ATLASSERT(ppCmd);

	Lock();

	HRESULT hr = S_OK;

	if (m_pTail)
	{
		*ppCmd = m_pTail->pCmd;
		(*ppCmd)->AddRef();
	}
	else
	{
		hr = S_FALSE;
	}

	Unlock();

	return hr;
}

HRESULT RpcQueue::Lookup(RpcCommandType type, RPC_COOKIE cookie, RpcCommand** ppCmd, BOOL bRemove)
{
	HRESULT hr = S_FALSE;

	Lock();

	_Node* pNode = m_pHead;
	for (; pNode; pNode = pNode->pNext)
	{
		if (pNode->pCmd->m_type == type && pNode->pCmd->m_cookie == cookie)
		{
			hr = S_OK;

			if (ppCmd)
			{
				*ppCmd = pNode->pCmd;
				(*ppCmd)->AddRef();
			}

			if (bRemove)
				_Remove(pNode);

			break;
		}
	}

	Unlock();

	return hr;
}

void RpcQueue::Remove(RpcCommand* pCmd)
{
	ATLASSERT(pCmd);

	Lock();

	_Node* pNode = m_pHead;
	for (; pNode; pNode = pNode->pNext)
	{
		if (pNode->pCmd == pCmd)
		{
			_Remove(pNode);

			break;
		}
	}

	Unlock();
}

HRESULT RpcQueue::RemoveHead(RpcCommand** ppCmd)
{
	Lock();

	HRESULT hr = S_OK;

	_Node* pNode = m_pHead;
	if (pNode)
	{
		m_pHead = pNode->pNext;
		if (m_pHead)
			m_pHead->pPrev = nullptr;
		else
			m_pTail = nullptr;

		--m_count;

		if (ppCmd)
			*ppCmd = pNode->pCmd;
		else
			pNode->pCmd->Release();

		pNode->pCmd = nullptr;

		pNode->pNext = m_pCache;

		m_pCache = pNode;
	}
	else if (ppCmd)
	{
		*ppCmd = nullptr;
		hr = S_FALSE;
	}

	Unlock();

	return hr;
}

HRESULT RpcQueue::RemoveTail(RpcCommand** ppCmd)
{
	Lock();

	HRESULT hr = S_OK;

	_Node* pNode = m_pTail;
	if (pNode)
	{
		m_pTail = pNode->pPrev;
		if (m_pTail)
			m_pTail->pNext = nullptr;
		else
			m_pHead = nullptr;

		--m_count;

		if (ppCmd)
			*ppCmd = pNode->pCmd;
		else
			pNode->pCmd->Release();

		pNode->pCmd = nullptr;

		pNode->pPrev = nullptr;
		pNode->pNext = m_pCache;

		m_pCache = pNode;
	}
	else if (ppCmd)
	{
		*ppCmd = nullptr;
		hr = S_FALSE;
	}

	Unlock();

	return hr;
}

void RpcQueue::RemoveAll()
{
	Lock();

	_RemoveAll();

	Unlock();
}

ULONG RpcQueue::GetCount()
{
	Lock();

	ULONG count = m_count;

	Unlock();

	return count;
}

void RpcQueue::_Remove(_Node* pNode)
{
	if (pNode->pPrev)
		pNode->pPrev->pNext = pNode->pNext;
	else
		m_pHead = pNode->pNext;

	if (pNode->pNext)
		pNode->pNext->pPrev = pNode->pPrev;
	else
		m_pTail = pNode->pPrev;

	pNode->pCmd->Release();

	pNode->pPrev = nullptr;
	pNode->pNext = m_pCache;
	m_pCache = pNode;

	--m_count;
}

void RpcQueue::_RemoveAll()
{
	_Node* pTemp = nullptr;
	_Node* pNode = m_pHead;
	while (pNode)
	{
		pTemp = pNode;
		pNode = pNode->pNext;

		pTemp->pCmd->Release();
		delete pTemp;
	}

	pNode = m_pCache;
	while (pNode)
	{
		pTemp = pNode;
		pNode = pNode->pNext;

		delete pTemp;
	}

	m_pHead = nullptr;
	m_pTail = nullptr;
	m_pCache = nullptr;

	m_count = 0;
}