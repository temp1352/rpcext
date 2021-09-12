#include "StdAfx.h"
#include "ProxyManagerList.h"

#include "RpcExt.h"

#include "HashMap.h"

// ProxyManagerList

ProxyManagerList::ProxyManagerList()
{
	m_count = 0;
	m_binCount = 0;

	m_ppBins = nullptr;
	m_pCache = nullptr;
}

ProxyManagerList::~ProxyManagerList()
{
}

HRESULT ProxyManagerList::Initialize()
{
	HRESULT hr = m_cs.Init();

	return hr;
}

void ProxyManagerList::Uninitialize()
{
	RemoveAll();
}

UINT _Hash(REFGUID cid, OID oid)
{
	return RpcHashMap::Hash(cid) << 5 | oid;
}

HRESULT ProxyManagerList::GetProxyManager(REFGUID cid, OID oid, ProxyManager** ppManager)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManagerList::GetProxyManager - cid : %ws, oid : %I64u", GuidString(cid).m_str, oid);

	ATLASSERT(ppManager);

	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	if (m_ppBins && m_binCount)
	{
		UINT hash = _Hash(cid, oid);
		UINT binIdx = hash % m_binCount;
		auto pNode = m_ppBins[binIdx];
		for (; pNode; pNode = pNode->pNext)
		{
			if (InlineIsEqualGUID(pNode->pManager->m_cid, cid) && pNode->pManager->m_oid == oid)
			{
				*ppManager = ComApi::AddRef(pNode->pManager);
				break;
			}
		}
	}

	HRESULT hr = *ppManager ? S_OK : S_FALSE;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"ProxyManagerList::GetProxyManager - end, hr : #%08x, count : %d", *ppManager, m_count);

	return hr;
}

HRESULT ProxyManagerList::Add(ProxyManager* pManager)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManagerList::Add");

	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	HRESULT hr = InitBins();
	if (FAILED(hr))
		return hr;

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
			return E_OUTOFMEMORY;
		}
	}

	pNode->hash = _Hash(pManager->m_cid, pManager->m_oid);
	pNode->pManager = ComApi::AddRef(pManager);

	UINT binIdx = pNode->hash % m_binCount;
	pNode->pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = pNode;

	++m_count;

	return S_OK;
}

BOOL ProxyManagerList::Remove(ProxyManager* pManager)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManagerList::Remove");

	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	if (!m_ppBins)
		return FALSE;

	UINT hash = _Hash(pManager->m_cid, pManager->m_oid);
	UINT binIdx = hash % m_binCount;

	_Node** ppNode = &m_ppBins[binIdx];
	for (; *ppNode; ppNode = &(*ppNode)->pNext)
	{
		if ((*ppNode)->pManager == pManager)
		{
			_Node* pNode = *ppNode;
			*ppNode = pNode->pNext;

			pNode->pManager->Release();
			pNode->pManager = nullptr;

			pNode->pNext = m_pCache;
			m_pCache = pNode;

			--m_count;

			return TRUE;
		}
	}

	return FALSE;
}

void ProxyManagerList::RemoveAll()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManagerList::RemoveAll");

	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	if (m_ppBins)
	{
		for (ULONG i = 0; i < m_binCount; ++i)
		{
			_Node *pNode = m_ppBins[i], *pTemp;
			while (pNode)
			{
				pTemp = pNode;
				pNode = pNode->pNext;

				pTemp->pManager->_Release();

				delete pTemp;
			}
		}

		free(m_ppBins);
		m_ppBins = nullptr;

		m_binCount = 0;
	}

	auto pNode = m_pCache;
	while (pNode)
	{
		auto pTemp = pNode;
		pNode = pNode->pNext;

		delete pTemp;
	}

	m_pCache = nullptr;

	m_count = 0;
}

HRESULT ProxyManagerList::InitBins()
{
	if (!m_ppBins)
	{
		ULONG binCount = RpcHashMap::BinSize(1000);

		m_ppBins = (_Node**)malloc(binCount * sizeof(_Node*));
		if (!m_ppBins)
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return E_OUTOFMEMORY;
		}

		memset(m_ppBins, 0, binCount * sizeof(_Node*));

		m_binCount = binCount;
	}

	return S_OK;
}