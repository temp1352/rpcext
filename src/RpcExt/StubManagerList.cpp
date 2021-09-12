#include "StdAfx.h"
#include "StubManagerList.h"

// StubManagerList

StubManagerList::StubManagerList()
{
	m_init = false;

	m_binCount = 0;
	m_ppBins = nullptr;
}

StubManagerList::~StubManagerList()
{
	_Clear();
}

HRESULT StubManagerList::Initialize()
{
	HRESULT hr = m_cs.Init();
	if (hr != S_OK)
		return hr;

	m_init = true;

	return hr;
}

void StubManagerList::Uninitialize()
{
	if (!m_init)
		return;

	Clear();
}

HRESULT StubManagerList::CreateStubManager(OID oid, IUnknown* pUnk, APTTYPE type, DWORD dwThreadId, StubManager** ppStubManager)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManagerList::CreateStubManager");

	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	HRESULT hr = Lookup(oid, ppStubManager);
	if (hr == S_OK)
		return hr;

	hr = StubManager::_CreateInstance(__uuidof(StubManager), (void**)ppStubManager);
	if (hr != S_OK)
		return hr;

	hr = (*ppStubManager)->Create(oid, pUnk, type, dwThreadId);
	if (FAILED(hr))
	{
		(*ppStubManager)->Release();
		*ppStubManager = nullptr;

		return hr;
	}

	hr = Insert(*ppStubManager);
	if (FAILED(hr))
	{
		(*ppStubManager)->Release();
		*ppStubManager = nullptr;

		return hr;
	}

	(*ppStubManager)->AddPrivateRef();

	return hr;
}

void StubManagerList::ReleaseStubManager(StubManager* pStubManager)
{
	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	DWORD ref = pStubManager->ReleasePrivateRef();

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManagerList::ReleaseStubManager, privateRef : %d", ref);

	if (ref == 0)
	{
		Remove(pStubManager->m_oid);
		pStubManager->Close();
	}
}

HRESULT StubManagerList::Insert(StubManager* pStubManager)
{
	if (!pStubManager)
		return E_POINTER;

	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	if (!m_ppBins)
	{
		HRESULT hr = InitBins();
		if (hr != S_OK)
			return hr;
	}

	_Node* pNode = new _Node;
	if (!pNode)
		return E_OUTOFMEMORY;

	pStubManager->AddRef();

	pNode->oid = pStubManager->GetOID();
	pNode->pStubManager = pStubManager;

	int binIdx = pNode->oid % m_binCount;

	pNode->pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = pNode;

	return S_OK;
}

HRESULT StubManagerList::Lookup(OID oid, StubManager** ppStubManager)
{
	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	if (!ppStubManager)
		return E_POINTER;

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	int binIdx = oid % m_binCount;

	_Node* pNode = m_ppBins[binIdx];
	while (pNode)
	{
		if (pNode->oid == oid)
		{
			*ppStubManager = pNode->pStubManager;
			(*ppStubManager)->AddRef();
			(*ppStubManager)->AddPrivateRef();

			return S_OK;
		}

		pNode = pNode->pNext;
	}

	return S_FALSE;
}

HRESULT StubManagerList::Remove(OID oid)
{
	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	int binIdx = oid % m_binCount;

	_Node *pNode = m_ppBins[binIdx], *pPrev = nullptr;
	while (pNode)
	{
		if (pNode->oid == oid)
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

	pNode->pStubManager->Release();
	delete pNode;

	return S_OK;
}

HRESULT StubManagerList::InitBins()
{
	ATLASSERT(!m_ppBins);

	static const UINT _anPrimes[] = {
		17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
		1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
		13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
		131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
		832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
		5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
	};

	ULONG binCount = 409;

	m_ppBins = new _Node*[binCount];
	if (!m_ppBins)
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return E_OUTOFMEMORY;
	}

	memset(m_ppBins, 0, sizeof(_Node*) * binCount);

	m_binCount = binCount;

	return S_OK;
}

void StubManagerList::Clear()
{
	CComCritSecLock<CComAutoDeleteCriticalSection> lock(m_cs);

	_Clear();
}

void StubManagerList::_Clear()
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

				pTemp->pStubManager->Release();

				delete pTemp;
			}
		}

		delete[] m_ppBins;
		m_ppBins = nullptr;
		m_binCount = 0;
	}
}