#include "stdafx.h"
#include "GuidMap.h"

#include "HashMap.h"

// GuidMap

GuidMap::GuidMap()
{
	m_count = 0;
	m_binCount = 0;
	m_ppBins = nullptr;
}

GuidMap::~GuidMap()
{
	_Clear();
}

HRESULT GuidMap::Insert(REFGUID key, IUnknown* pUnk, _Position* pPos)
{
	if (!pUnk)
		return E_POINTER;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	HRESULT hr = S_OK;

	if (!m_ppBins)
	{
		hr = InitBins();
		if (FAILED(hr))
			return hr;
	}

	_Node* pNode = new _Node;
	if (!pNode)
		return E_OUTOFMEMORY;

	pUnk->AddRef();
	pNode->pUnk = pUnk;

	pNode->hash = RpcHashMap::Hash(key);
	pNode->key = key;

	UINT binIdx = pNode->hash % m_binCount;

	pNode->pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = pNode;

	++m_count;

	if (pPos)
	{
		*pPos = pNode;
	}

	return S_OK;
}

HRESULT GuidMap::Lookup(REFGUID key, REFIID riid, void** ppv)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	UINT hash = RpcHashMap::Hash(key);
	int binIdx = hash % m_binCount;

	_Node* pNode = m_ppBins[binIdx];
	while (pNode)
	{
		if (pNode->hash == hash && InlineIsEqualGUID(pNode->key, key))
		{
			if (ppv)
			{
				return pNode->pUnk->QueryInterface(riid, ppv);
			}

			return S_OK;
		}

		pNode = pNode->pNext;
	}

	return S_FALSE;
}

HRESULT GuidMap::Remove(REFGUID key, REFIID riid, void** ppv)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	UINT hash = RpcHashMap::Hash(key);
	int binIdx = hash % m_binCount;
	auto ppNode = &m_ppBins[binIdx];
	
	for(; *ppNode; ppNode = &(*ppNode)->pNext)
	{
		if ((*ppNode)->hash == hash && InlineIsEqualGUID((*ppNode)->key, key))
		{
			break;
		}
	}

	if (!*ppNode)
		return E_INVALIDARG;

	auto pNode = *ppNode;
	*ppNode = pNode->pNext;
	
	HRESULT hr = S_OK;

	if (ppv)
	{
		hr = pNode->pUnk->QueryInterface(riid, ppv);
	}

	pNode->pUnk->Release();
	delete pNode;

	--m_count;

	return hr;
}

HRESULT GuidMap::Remove(_Position pos)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	_Node** ppNode = nullptr;

	for (int binIdx = 0; binIdx < m_binCount; ++binIdx)
	{
		ppNode = &m_ppBins[binIdx];
		for(; *ppNode; ppNode = &(*ppNode)->pNext)
		{
			if (*ppNode == pos)
			{
				break;
			}
		}

		if (*ppNode)
			break;
	}

	if (!*ppNode)
		return E_INVALIDARG;

	auto pNode = *ppNode;
	*ppNode = pNode->pNext;

	pNode->pUnk->Release();
	delete pNode;

	--m_count;

	return S_OK;
}

HRESULT GuidMap::InitBins()
{
	ATLASSERT(!m_ppBins);

	ULONG binCount = RpcHashMap::BinSize(100);

	m_ppBins = (_Node**)malloc(binCount * sizeof(_Node*));
	if (!m_ppBins)
	{
		SetLastError(ERROR_OUTOFMEMORY);

		return E_OUTOFMEMORY;
	}

	memset(m_ppBins, 0, sizeof(_Node*) * binCount);

	m_binCount = binCount;

	return S_OK;
}

void GuidMap::Clear()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Clear();
}

void GuidMap::_Clear()
{
	if (m_ppBins)
	{
		_Node *pTemp, *pNode;

		for (ULONG i = 0; i < m_binCount; ++i)
		{
			pNode = m_ppBins[i];
			while (pNode)
			{
				pTemp = pNode;
				pNode = pNode->pNext;

				pTemp->pUnk->Release();
				delete pTemp;
			}
		}

		free(m_ppBins);
		m_ppBins = nullptr;

		m_binCount = 0;
		m_count = 0;
	}
}