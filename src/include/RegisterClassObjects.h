#pragma once

#include "RpcShell.h"

struct _RE_REGISTER_NODE
{
	DWORD dwRegister;
	_RE_REGISTER_NODE* pPrev;

	_RE_REGISTER_NODE()
	{
		dwRegister = 0;
		pPrev = nullptr;
	}
};

__declspec(selectany) _RE_REGISTER_NODE* _pLastRegister = nullptr;

inline HRESULT _RegisterClassObject(_ATL_OBJMAP_ENTRY_EX* pEntry, DWORD dwClsContext, DWORD dwFlags, DWORD usage)
{
	if (pEntry->pfnGetClassObject == nullptr)
		return S_OK;

	IUnknown* p = nullptr;
	HRESULT hr = pEntry->pfnGetClassObject(pEntry->pfnCreateInstance, __uuidof(IUnknown), (LPVOID*)&p);
	if (FAILED(hr) || p == nullptr)
		return hr;

	hr = CoRegisterClassObject(*pEntry->pclsid, p, dwClsContext, dwFlags, &pEntry->pCache->dwRegister);

	auto pNode = new _RE_REGISTER_NODE;
	if (pNode)
	{
		hr = RE_RegisterClassObject(*pEntry->pclsid, p, dwClsContext, dwFlags, usage, &pNode->dwRegister);
		if (SUCCEEDED(hr))
		{
			pNode->pPrev = _pLastRegister;
			_pLastRegister = pNode;
		}
		else
		{
			delete pNode;
		}
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	p->Release();

	return hr;
}

inline HRESULT RE_RegisterClassObjects(_ATL_COM_MODULE* pComModule, DWORD dwClsContext, DWORD dwFlags, DWORD usage)
{
	ATLASSERT(pComModule != nullptr);
	if (pComModule == nullptr)
		return E_INVALIDARG;

	HRESULT hr = S_FALSE;
	for (_ATL_OBJMAP_ENTRY_EX** ppEntry = pComModule->m_ppAutoObjMapFirst; ppEntry < pComModule->m_ppAutoObjMapLast && SUCCEEDED(hr); ppEntry++)
	{
		if (*ppEntry != nullptr)
		{
			hr = _RegisterClassObject(*ppEntry, dwClsContext, dwFlags, usage);
		}
	}

	return hr;
}

inline HRESULT RE_RevokeClassObjects(_ATL_COM_MODULE* pComModule)
{
	ATLASSERT(pComModule != nullptr);
	if (pComModule == nullptr)
		return E_INVALIDARG;

	HRESULT hr = S_OK;
	for (_ATL_OBJMAP_ENTRY_EX** ppEntry = pComModule->m_ppAutoObjMapFirst; ppEntry < pComModule->m_ppAutoObjMapLast && hr == S_OK; ppEntry++)
	{
		if (*ppEntry != nullptr)
		{
			hr = (*ppEntry)->RevokeClassObject();
		}
	}

	_RE_REGISTER_NODE *pNode = _pLastRegister, *pTemp;
	while (pNode)
	{
		RE_RevokeClassObject(pNode->dwRegister);

		pTemp = pNode;
		pNode = pNode->pPrev;

		delete pTemp;
	}

	_pLastRegister = nullptr;

	return hr;
}