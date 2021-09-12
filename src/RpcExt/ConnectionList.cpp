#include "StdAfx.h"
#include "ConnectionList.h"

#include "RpcExt.h"

// ConnectionList

ConnectionList::ConnectionList()
{
	m_count = 0;
	m_binCount = 0;

	m_pHead = nullptr;
	m_pTail = nullptr;
	m_pCache = nullptr;

	m_ppBins = nullptr;
}

ConnectionList::~ConnectionList()
{
	RemoveAll();
}

HRESULT ConnectionList::Initialize(ULONG binCount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins)
	{
		m_ppBins = (_Node**)malloc(binCount * sizeof(_Node*));
		if (!m_ppBins)
		{
			return E_OUTOFMEMORY;
		}

		memset(m_ppBins, 0, sizeof(_Node*) * binCount);

		m_binCount = binCount;
	}

	return S_OK;
}

void ConnectionList::Term()
{
	RemoveAll();
}

HRESULT ConnectionList::GetConnection(REFGUID cid, RpcConnection** ppConnection)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"ConnectionList::GetConnection - cid : %ws", GuidString(cid).m_str);

	ATLASSERT(ppConnection);

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Node* pNode = m_pHead;
	for (; pNode; pNode = pNode->pNext)
	{
		if (InlineIsEqualGUID(pNode->pConn->m_cid, cid))
		{
			*ppConnection = ComApi::AddRef(pNode->pConn);
			break;
		}
	}

	HRESULT hr = *ppConnection ? S_OK : S_FALSE;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"ConnectionList::GetConnection - hr : #%08x", hr);

	return hr;
}

HRESULT ConnectionList::GetConnection(ULONG_PTR key, bool remove, RpcConnection** ppConnection)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"ConnectionList::GetConnection - key : #%08x, remove : %d", key, remove);

	ATLASSERT(ppConnection);

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins)
	{
		return S_FALSE;
	}

	HRESULT hr = S_FALSE;

	int index = key % m_binCount;

	_Node** ppNode = &m_ppBins[index];
	for (; *ppNode; ppNode = &(*ppNode)->pLink)
	{
		if ((*ppNode)->pConn->m_aid == key)
			break;
	}

	if (*ppNode)
	{
		hr = S_OK;

		_Node* pNode = *ppNode;
		*ppConnection = pNode->pConn;

		if (remove)
		{
			*ppNode = pNode->pLink;

			pNode->pConn = nullptr;

			if (pNode->pPrev)
				pNode->pPrev->pNext = pNode->pNext;
			else
				m_pHead = pNode->pNext;

			if (pNode->pNext)
				pNode->pNext->pPrev = pNode->pPrev;
			else
				m_pTail = pNode->pPrev;

			pNode->pPrev = nullptr;
			pNode->pNext = m_pCache;
			pNode->pLink = nullptr;

			m_pCache = pNode;

			--m_count;
		}
		else
		{
			(*ppConnection)->AddRef();
		}
	}

	return hr;
}

HRESULT ConnectionList::Add(RpcConnection* pConn)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Node* pNode = m_pCache;
	if (pNode)
	{
		m_pCache = pNode->pNext;
		pNode->pNext = nullptr;
	}
	else
	{
		pNode = new _Node;
		if (!pNode)
		{
			return E_OUTOFMEMORY;
		}
	}

	pConn->AddRef();
	pNode->pConn = pConn;

	pNode->pPrev = m_pTail;
	pNode->pNext = nullptr;

	if (m_pTail)
		m_pTail->pNext = pNode;
	else
		m_pHead = pNode;

	m_pTail = pNode;

	int index = pConn->m_aid % m_binCount;
	pNode->pLink = m_ppBins[index];
	m_ppBins[index] = pNode;

	++m_count;

	return S_OK;
}

BOOL ConnectionList::Remove(ULONG_PTR key)
{
	BOOL ret = FALSE;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins)
	{
		return FALSE;
	}

	int index = key % m_binCount;

	_Node** ppNode = &m_ppBins[index];
	for (; *ppNode; ppNode = &(*ppNode)->pLink)
	{
		if ((*ppNode)->pConn->m_aid == key)
			break;
	}

	if (*ppNode)
	{
		_Node* pNode = *ppNode;
		*ppNode = pNode->pLink;

		pNode->pConn->Release();
		pNode->pConn = nullptr;

		if (pNode->pPrev)
			pNode->pPrev->pNext = pNode->pNext;
		else
			m_pHead = pNode->pNext;

		if (pNode->pNext)
			pNode->pNext->pPrev = pNode->pPrev;
		else
			m_pTail = pNode->pPrev;

		pNode->pPrev = nullptr;
		pNode->pNext = m_pCache;
		pNode->pLink = nullptr;

		m_pCache = pNode;

		--m_count;

		ret = TRUE;
	}

	return ret;
}

BOOL ConnectionList::Remove(RpcConnection* pConn)
{
	BOOL ret = FALSE;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins)
	{
		return FALSE;
	}

	int index = pConn->m_aid % m_binCount;

	_Node** ppNode = &m_ppBins[index];
	for (; *ppNode; ppNode = &(*ppNode)->pLink)
	{
		if ((*ppNode)->pConn == pConn)
			break;
	}

	if (*ppNode)
	{
		_Node* pNode = *ppNode;
		*ppNode = pNode->pLink;

		pNode->pConn->Release();
		pNode->pConn = nullptr;

		if (pNode->pPrev)
			pNode->pPrev->pNext = pNode->pNext;
		else
			m_pHead = pNode->pNext;

		if (pNode->pNext)
			pNode->pNext->pPrev = pNode->pPrev;
		else
			m_pTail = pNode->pPrev;

		pNode->pPrev = nullptr;
		pNode->pNext = m_pCache;
		pNode->pLink = nullptr;

		m_pCache = pNode;

		--m_count;

		ret = TRUE;
	}

	return ret;
}

void ConnectionList::RemoveAll()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Node* pTemp = nullptr;
	_Node* pNode = m_pHead;
	while (pNode)
	{
		pTemp = pNode;
		pNode = pNode->pNext;

		pTemp->pConn->Release();

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

	if (m_ppBins)
	{
		free(m_ppBins);
		m_ppBins = nullptr;
	}

	m_count = 0;
	m_binCount = 0;
}

void ConnectionList::Remove(_Node* pNode)
{
	int index = pNode->pConn->m_aid % m_binCount;
	_Node** ppNode = &m_ppBins[index];
	for (; *ppNode; ppNode = &(*ppNode)->pLink)
	{
		if (*ppNode == pNode)
			break;
	}

	*ppNode = pNode->pLink;

	if (pNode->pPrev)
		pNode->pPrev->pNext = pNode->pNext;
	else
		m_pHead = pNode->pNext;

	if (pNode->pNext)
		pNode->pNext->pPrev = pNode->pPrev;
	else
		m_pTail = pNode->pPrev;

	pNode->pConn->Release();
	pNode->pConn = nullptr;

	pNode->pPrev = nullptr;
	pNode->pNext = m_pCache;
	pNode->pLink = nullptr;

	m_pCache = pNode;
}

void ConnectionList::CloseAll()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Node* pTemp = nullptr;
	_Node* pNode = m_pHead;
	while (pNode)
	{
		pTemp = pNode;
		pNode = pNode->pNext;

		pTemp->pConn->Close();
		pTemp->pConn->Release();

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

	if (m_ppBins)
	{
		free(m_ppBins);
		m_ppBins = nullptr;
	}

	m_count = 0;
	m_binCount = 0;
}