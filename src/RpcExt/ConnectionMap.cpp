#include "stdafx.h"
#include "ConnectionMap.h"

#include "HashMap.h"

// RpcConnectionMap

RpcConnectionMap::RpcConnectionMap()
{
	m_binCount = 0;
	m_ppBins = nullptr;
}

RpcConnectionMap::~RpcConnectionMap()
{
	_Clear();
}

HRESULT RpcConnectionMap::Initialize()
{
	return S_OK;
}

void RpcConnectionMap::Uninitialize()
{
	Clear();
}

HRESULT RpcConnectionMap::Insert(RpcClientConnection* pConnection)
{
	if (!pConnection)
		return E_POINTER;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	HRESULT hr = Lookup(pConnection->m_cid, nullptr);
	if (hr == S_OK)
		return S_FALSE;

	if (!m_ppBins)
	{
		hr = InitBins();
		if (hr != S_OK)
			return hr;
	}

	pConnection->m_hash = RpcHashMap::Hash(pConnection->m_cid);

	int binIdx = pConnection->m_hash % m_binCount;

	pConnection->m_pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = ComApi::AddRef(pConnection);

	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnectionMap::Insert, hash : %d, binIdx : %d, cid = ", pConnection->m_hash, binIdx);
	//_logFile.WriteBinary(LogCategory::Misc, LogLevel::General, LogOption::Std, &pConnection->m_cid, sizeof(GUID));

	return S_OK;
}

HRESULT RpcConnectionMap::Lookup(REFGUID cid, RpcClientConnection** ppConnection)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	UINT hash = RpcHashMap::Hash(cid);
	int binIdx = hash % m_binCount;

	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnectionMap::Lookup, hash : %d, binIdx : %d, cid = ", hash, binIdx);
	//_logFile.WriteBinary(LogCategory::Misc, LogLevel::General, LogOption::Std, &cid, sizeof(GUID));

	RpcClientConnection* pNode = m_ppBins[binIdx];
	while (pNode)
	{
		if (pNode->m_hash == hash && InlineIsEqualGUID(pNode->m_cid, cid))
		{
			if (ppConnection)
			{
				*ppConnection = ComApi::AddRef(pNode);
			}

			return S_OK;
		}

		pNode = pNode->m_pNext;
	}

	return S_FALSE;
}

BOOL RpcConnectionMap::Remove(RpcClientConnection* pConnection)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return FALSE;

	int binIdx = pConnection->m_hash % m_binCount;
	
	RpcClientConnection* pNode = m_ppBins[binIdx];
	RpcClientConnection* pPrev = nullptr;

	while (pNode)
	{
		if (pNode == pConnection)
		{
			break;
		}

		pPrev = pNode;
		pNode = pNode->m_pNext;
	}

	if (pNode == nullptr)
		return FALSE;

	if (pPrev)
		pPrev->m_pNext = pNode->m_pNext;
	else
		m_ppBins[binIdx] = pNode->m_pNext;

	pConnection->m_hash = 0;
	pConnection->m_pNext = nullptr;
	pConnection->Release();

	return TRUE;
}

HRESULT RpcConnectionMap::InitBins()
{
	ATLASSERT(!m_ppBins);

	ULONG binCount = RpcHashMap::BinSize(1000);

	m_ppBins = (RpcClientConnection**)malloc(binCount * sizeof(RpcClientConnection*));
	if (!m_ppBins)
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return E_OUTOFMEMORY;
	}

	memset(m_ppBins, 0, binCount * sizeof(RpcClientConnection*));

	m_binCount = binCount;

	return S_OK;
}

void RpcConnectionMap::Clear()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Clear();
}

void RpcConnectionMap::_Clear()
{
	if (m_ppBins)
	{
		for (ULONG i = 0; i < m_binCount; ++i)
		{
			RpcClientConnection *pNode = m_ppBins[i], *pTemp;
			while (pNode)
			{
				pTemp = pNode;
				pNode = pNode->m_pNext;

				pTemp->m_hash = 0;
				pTemp->m_pNext = nullptr;
				pTemp->_Release();
			}
		}

		free(m_ppBins);
		m_ppBins = nullptr;

		m_binCount = 0;
	}
}