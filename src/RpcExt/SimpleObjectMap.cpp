#include "StdAfx.h"
#include "SimpleObjectMap.h"

// SimpleObjectMap

SimpleObjectMap::SimpleObjectMap()
{
	m_binCount = 0;
	m_ppBins = nullptr;
}

SimpleObjectMap::~SimpleObjectMap()
{
	Clear();
}

HRESULT SimpleObjectMap::Insert(REFGUID rguid, IUnknown* pUnk)
{
	if (!pUnk)
		return E_POINTER;

	_Node* pNode = GetNode(rguid);
	if (pNode)
	{
		pUnk->AddRef();
		pNode->pUnk->Release();
		pNode->pUnk = pUnk;

		return S_OK;
	}

	if (!m_ppBins)
	{
		HRESULT hr = InitBins();
		if (hr != S_OK)
			return hr;
	}

	pNode = new _Node;
	if (!pNode)
		return E_OUTOFMEMORY;

	pNode->pUnk = pUnk;
	pNode->pUnk->AddRef();

	pNode->guid = rguid;
	pNode->hash = Hash(rguid);

	int binIdx = pNode->hash % m_binCount;

	pNode->pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = pNode;

	return S_OK;
}

HRESULT SimpleObjectMap::Lookup(REFGUID rguid, REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;

	_Node* pNode = GetNode(rguid);
	if (pNode)
		return pNode->pUnk->QueryInterface(riid, ppv);

	return S_FALSE;
}

HRESULT SimpleObjectMap::Remove(REFGUID rguid)
{
	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	UINT hash = Hash(rguid);
	int binIdx = hash % m_binCount;
	;

	_Node* pNode = m_ppBins[binIdx];
	_Node* pPrev = nullptr;
	while (pNode)
	{
		if (pNode->hash == hash && InlineIsEqualGUID(pNode->guid, rguid))
		{
			break;
		}

		pPrev = pNode;
		pNode = pNode->pNext;
	}

	if (pNode == nullptr)
		return E_INVALIDARG;

	if (pPrev)
		pPrev->pNext = pNode->pNext;
	else
		m_ppBins[binIdx] = pNode->pNext;

	pNode->pUnk->Release();
	delete pNode;

	return S_OK;
}

SimpleObjectMap::_Node* SimpleObjectMap::GetNode(REFGUID rguid)
{
	if (!m_ppBins || !m_binCount)
		return nullptr;

	UINT hash = Hash(rguid);
	int binIdx = hash % m_binCount;
	;

	_Node* pNode = m_ppBins[binIdx];
	while (pNode)
	{
		if (pNode->hash == hash && InlineIsEqualGUID(pNode->guid, rguid))
		{
			return pNode;
		}

		pNode = pNode->pNext;
	}

	return nullptr;
}

UINT SimpleObjectMap::Hash(REFGUID guid)
{
	const DWORD* pdwData = reinterpret_cast<const DWORD*>(&guid);

	return (pdwData[0] ^ pdwData[1] ^ pdwData[2] ^ pdwData[3]);
}

HRESULT SimpleObjectMap::InitBins()
{
	ATLASSERT(!m_ppBins);

	ULONG nBin = 41;

	m_ppBins = new _Node*[nBin];
	if (!m_ppBins)
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return E_OUTOFMEMORY;
	}

	memset(m_ppBins, 0, sizeof(_Node*) * nBin);

	m_binCount = nBin;

	return S_OK;
}

void SimpleObjectMap::Clear()
{
	if (m_ppBins)
	{
		for (ULONG i = 0; i < m_binCount; ++i)
		{
			_Node *pNode = m_ppBins[i], *pTemp;
			while (pNode)
			{
				pTemp = pNode;
				pNode = pNode->pNext;
				pTemp->pUnk->Release();
				delete pTemp;
			}
		}

		delete[] m_ppBins;
		m_ppBins = nullptr;
		m_binCount = 0;
	}
}