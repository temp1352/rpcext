#pragma once

#include "ThreadInfo.h"

// RpcThreadStorage

class RpcThreadStorage
{
public:
	RpcThreadStorage();
	~RpcThreadStorage();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT InitThreadInfo(APTTYPE type);
	void UninitThreadInfo();

	HRESULT GetThreadInfo(RpcThreadInfo** ppThreadInfo);
	HRESULT GetThreadInfo(DWORD threadId, RpcThreadInfo** ppThreadInfo);

	void Add(RpcThreadInfo* pThreadInfo);
	void Remove(RpcThreadInfo* pThreadInfo);
	void Clear();

	HRESULT InsertValue(REFGUID rguid, IUnknown* pUnk);
	HRESULT LookupValue(REFGUID rguid, REFIID riid, void** ppv);

	HRESULT RemoveValue(REFGUID rguid);
	void ClearValues();

protected:
	RpcThreadInfo* CreateThreadInfo(APTTYPE type);

protected:
	BOOL m_init;

	CComAutoDeleteCriticalSection m_cs;

	DWORD m_tlsIndex;

	RpcThreadInfo* m_pHead;
};