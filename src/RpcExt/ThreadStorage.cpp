#include "StdAfx.h"
#include "ThreadStorage.h"

// RpcThreadStorage

RpcThreadStorage::RpcThreadStorage()
{
	m_init = FALSE;

	m_tlsIndex = 0;

	m_pHead = nullptr;
}

RpcThreadStorage::~RpcThreadStorage()
{
}

HRESULT RpcThreadStorage::Initialize()
{
	HRESULT hr = m_cs.Init();
	if (FAILED(hr))
		return hr;

	m_tlsIndex = TlsAlloc();
	if (m_tlsIndex == TLS_OUT_OF_INDEXES)
		return AtlHresultFromLastError();

	InterlockedExchange((LONG*)&m_init, FALSE);

	return S_OK;
}

void RpcThreadStorage::Uninitialize()
{
	if (m_tlsIndex != TLS_OUT_OF_INDEXES)
	{
		TlsFree(m_tlsIndex);
		m_tlsIndex = 0;

		Clear();
	}
}

HRESULT RpcThreadStorage::InitThreadInfo(APTTYPE type)
{
	auto pThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (!pThreadInfo)
	{
		pThreadInfo = CreateThreadInfo(type);
		if (!pThreadInfo)
			return AtlHresultFromLastError();
	}

	return S_OK;
}

void RpcThreadStorage::UninitThreadInfo()
{
	if (!m_init)
		return;

	m_cs.Lock();

	auto pThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (pThreadInfo)
	{
		TlsSetValue(m_tlsIndex, nullptr);

		pThreadInfo->Uninitialize();

		Remove(pThreadInfo);
	}

	m_cs.Unlock();
}

HRESULT RpcThreadStorage::GetThreadInfo(RpcThreadInfo** ppThreadInfo)
{
	ATLASSERT(ppThreadInfo);

	*ppThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (*ppThreadInfo)
	{
		(*ppThreadInfo)->AddRef();

		return S_OK;
	}

	return S_FALSE;
}

HRESULT RpcThreadStorage::GetThreadInfo(DWORD threadId, RpcThreadInfo** ppThreadInfo)
{
	ATLASSERT(ppThreadInfo);

	m_cs.Lock();

	RpcThreadInfo* pNode = m_pHead;
	while (pNode)
	{
		if (pNode->m_threadId == threadId)
			break;

		pNode = pNode->m_pNext;
	}

	HRESULT hr = S_FALSE;

	if (pNode)
	{
		*ppThreadInfo = pNode;
		pNode->AddRef();

		hr = S_OK;
	}

	m_cs.Unlock();

	return hr;
}

void RpcThreadStorage::Add(RpcThreadInfo* pThreadInfo)
{
	m_cs.Lock();

	pThreadInfo->AddRef();

	pThreadInfo->m_pNext = m_pHead;
	m_pHead = pThreadInfo;

	m_cs.Unlock();
}

void RpcThreadStorage::Remove(RpcThreadInfo* pThreadInfo)
{
	m_cs.Lock();

	RpcThreadInfo* pNode = m_pHead;
	RpcThreadInfo* pPrev = nullptr;
	while (pNode)
	{
		if (pNode == pThreadInfo)
			break;

		pPrev = pNode;
		pNode = pNode->m_pNext;
	}

	if (pPrev)
		pPrev->m_pNext = pNode->m_pNext;
	else
		m_pHead = pNode->m_pNext;

	pNode->Release();

	m_cs.Unlock();
}

void RpcThreadStorage::Clear()
{
	m_cs.Lock();

	RpcThreadInfo* pNode = m_pHead;
	while (pNode)
	{
		RpcThreadInfo* pTemp = pNode;
		pNode = pNode->m_pNext;

		pTemp->Release();
	}

	m_pHead = nullptr;

	m_cs.Unlock();
}

HRESULT RpcThreadStorage::InsertValue(REFGUID rguid, IUnknown* pUnk)
{
	if (!pUnk)
		return E_POINTER;

	auto pThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (!pThreadInfo)
	{
		pThreadInfo = CreateThreadInfo(APTTYPE_MTA);
		if (!pThreadInfo)
			return AtlHresultFromLastError();
	}

	return pThreadInfo->m_valueMap.Insert(rguid, pUnk);
}

HRESULT RpcThreadStorage::LookupValue(REFGUID rguid, REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;

	auto pThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (!pThreadInfo)
		return E_UNEXPECTED;

	return pThreadInfo->m_valueMap.Lookup(rguid, riid, ppv);
}

HRESULT RpcThreadStorage::RemoveValue(REFGUID rguid)
{
	auto pThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (!pThreadInfo)
		return E_UNEXPECTED;

	return pThreadInfo->m_valueMap.Remove(rguid);
}

void RpcThreadStorage::ClearValues()
{
	auto pThreadInfo = (RpcThreadInfo*)TlsGetValue(m_tlsIndex);
	if (pThreadInfo)
	{
		pThreadInfo->m_valueMap.Clear();
	}
}

RpcThreadInfo* RpcThreadStorage::CreateThreadInfo(APTTYPE type)
{
	auto pThreadInfo = new CComObject<RpcThreadInfo>;
	if (!pThreadInfo)
	{
		SetLastError(E_OUTOFMEMORY);

		return nullptr;
	}

	HRESULT hr = pThreadInfo->Initialize(type);
	if (FAILED(hr))
	{
		delete pThreadInfo;
		SetLastError(hr);

		return nullptr;
	}

	TlsSetValue(m_tlsIndex, pThreadInfo);

	Add(pThreadInfo);

	return pThreadInfo;
}
